#include <thread>
#include <iostream>
#include <time.h>
#include <string>
#include <vector>
#include <sstream>
#include <memory>
#include <mutex>

//Our Code
#include "sensors.cpp"
#include "station.cpp"

//Environmental Conditions! These are read by the "sensors"
auto ambientTemperature =    std::make_shared<float>(25.0f); //in celsius
auto ambientHumidity =       std::make_shared<float>(50.0f); //in %
auto ambientPressure =       std::make_shared<float>(1013.25f); // in kPa
auto windSpeed =          std::make_shared<float>(0.0f); //in m/s
auto windDirection =      std::make_shared<float>(0.0f); //in degrees
//######################################################
//OUTPUT(s)! This is shown on the "display" of the station
const std::string displayFile = "display.txt"; //File to read the ambient conditions from
//######################################################

std::mutex terminalMutex;

void writeActualData(std::mutex& m, std::shared_ptr<std::vector<sensor>> sensors);

int main(void){
    //Create the sensors
    auto sensors = std::make_shared<std::vector<sensor>>();
    sensors->emplace_back(-55, 125, 0.1f, ambientTemperature, std::string("°C")); //Temperature sensor
    sensors->emplace_back(0, 100, 0.5f, ambientHumidity, std::string("%")); //Humidity sensor
    sensors->emplace_back(300, 1200, 0.05f, ambientPressure, std::string("hPa")); //Pressure sensor
    sensors->emplace_back(0, 60, 0.3f, windSpeed, std::string("m/s")); //Wind speed sensor
    sensors->emplace_back(-180, 180, 2.0f, windDirection, std::string("°")); //Wind direction sensor


    //Start the station in a seperate thread
    WeatherStation station(sensors);
    std::thread stationThread(&WeatherStation::run, &station, std::ref(terminalMutex), time(nullptr));

    std::thread actualDataThread(writeActualData, std::ref(terminalMutex), sensors);

    std::this_thread::sleep_for(std::chrono::seconds(1));

    //update the ambient conditions from the terminal
    std::string input;
    while (true) {
        while(!terminalMutex.try_lock()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10)); // Wait for a bit before trying again
        }

        std::cout << "\033[" << 6 << ";1H"; // ANSI: Move cursor to line n, column 1
        std::cout << "\033[2K"; // ANSI: Clear entire line
        std::cout << "Enter new ambient conditions (" << sensors->size() << ") floats: \n";
        std::cout << "\033[2K"; // ANSI: Clear entire line

        terminalMutex.unlock();

        std::getline(std::cin, input);
        if (input == "exit") {
            break;
        }

        std::istringstream iss(input);
        float temp, humidity, pressure, windSpeedValue, windDirectionValue;
        if (iss >> temp >> humidity >> pressure >> windSpeedValue >> windDirectionValue) {
            *ambientTemperature = temp;
            *ambientHumidity = humidity;
            *ambientPressure = pressure;
            *windSpeed = windSpeedValue;
            *windDirection = windDirectionValue;
        }
    }

    return 0;
}

//write the actual data to the file
void writeActualData(std::mutex& m, std::shared_ptr<std::vector<sensor>> sensors){
    while(true){
        while(!terminalMutex.try_lock()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10)); // Wait for a bit before trying again
        }

        std::cout << "\033[" << 1 << ";1H"; // ANSI: Move cursor to line n, column 1
        std::cout << "\033[2K"; // ANSI: Clear entire line

        //get the current time
        time_t currentTime = std::time(nullptr);
        std::cout << std::put_time(std::localtime(&currentTime), "%Y-%m-%d %H:%M:%S");

        for(auto s : *sensors){
            std::cout << std::setprecision(3) << std::setw(10) << s.getActual() << " " << s.getUnits() << " ";
        }

        terminalMutex.unlock();

        std::cout << "\033[" << 7 << ";1H"; // ANSI: Move cursor to line n, column 1
        std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Sleep for half a second before writing again
    }
}