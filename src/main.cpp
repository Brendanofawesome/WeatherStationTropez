#include <thread>
#include <iostream>
#include <time.h>
#include <string>
#include <vector>
#include <sstream>
#include <memory>

//Our Code
#include "sensors.cpp"
#include "station.cpp"

//Environmental Conditions! These are read by the "sensors"
auto ambientTemperature =    std::make_shared<float>(25.0f); //in celsius
auto ambientHumidity =       std::make_shared<float>(50.0f); //in %
auto ambientPressure =       std::make_shared<float>(1013.25f); // in kPa
auto ambientWindSpeed =          std::make_shared<float>(0.0f); //in m/s
auto ambientWindDirection =      std::make_shared<float>(0.0f); //in degrees
//######################################################
//OUTPUT(s)! This is shown on the "display" of the station
const std::string displayFile = "display.txt"; //File to read the ambient conditions from
//######################################################

int main(void){
    //Create the sensors
    auto sensors = std::make_unique<std::vector<sensor>>();
    sensors->emplace_back(-55, 125, 0.1f, ambientTemperature, std::string("°C")); //Temperature sensor
    sensors->emplace_back(0, 100, 0.5f, ambientHumidity, std::string("%")); //Humidity sensor
    sensors->emplace_back(300, 1200, 0.05f, ambientPressure, std::string("hPa")); //Pressure sensor
    sensors->emplace_back(0, 60, 0.3f, ambientWindSpeed, std::string("m/s")); //Wind speed sensor
    sensors->emplace_back(-180, 180, 2.0f, ambientWindDirection, std::string("°")); //Wind direction sensor


    //Start the station in a seperate thread
    WeatherStation station(displayFile, std::move(sensors));
    std::thread stationThread(&WeatherStation::run, &station, time(0));
    //get the current time
    time_t now = time(0);

    std::this_thread::sleep_for(std::chrono::seconds(1));

    //update the ambient conditions from the terminal
    std::string input;
    while (true) {
        std::cout << "Enter new ambient conditions (temp, humidity, pressure, wind speed, wind direction): ";
        std::getline(std::cin, input);
        if (input == "exit") {
            break;
        }

        std::istringstream iss(input);
        float temp, humidity, pressure, windSpeed, windDirection;
        if (iss >> temp >> humidity >> pressure >> windSpeed >> windDirection) {
            *ambientTemperature = temp;
            *ambientHumidity = humidity;
            *ambientPressure = pressure;
            *ambientWindSpeed = windSpeed;
            *ambientWindDirection = windDirection;
        } else {
            std::cout << "Invalid input. Please enter three numbers." << std::endl;
        }
    }

    //Stop the station thread
    //station.stopStation();
    return 0;
}