#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <deque>
#include <string>
#include <iomanip>
#include <memory>
#include <thread>
#include <chrono>
#include "sensors.cpp"

class WeatherStation {
public:
    WeatherStation(const std::string& file, std::unique_ptr<std::vector<sensor>> inSensors): filePath(file), storage(inSensors->size()) {
        this->sensors = std::move(inSensors);
    }

    void run(time_t currentTime) {
        //open the datastorage file
        std::ofstream outFile("storage.txt"); // Open file and overwrite it

        time_t nextTime = currentTime + 1;
        std::cout << "Weather Station started." << std::endl;
        while (true) {
            print(currentTime);
            saveLog(currentTime, "storage.txt");

            //sleep until next time
            std::this_thread::sleep_until(std::chrono::system_clock::from_time_t(nextTime));

            currentTime = nextTime;
            nextTime = nextTime + 1;
        }
    }

private:
    void print(const time_t& currentTime){
        std::ofstream outFile(filePath); // Open file and overwrite it
        if (!outFile.is_open()) {
            throw std::runtime_error("Failed to open file: " + filePath);
        }

        outFile << "Weather Station Data\n";

        outFile << "Time: " << std::put_time(std::localtime(&currentTime), "%Y-%m-%d %H:%M:%S") << "\n";

        outFile << "Readings: ";
        for (size_t i = 0; i < sensors->size(); ++i) {
            float reading = (*sensors)[i](); // Get the current reading from the sensor
            outFile << std::setprecision(3) << std::setw(10) << std::fixed << reading << " " << (*sensors)[i].getUnits() << " ";

            // Update the deque with the new reading
            if (storage[i].size() == 10) {
                storage[i].pop_front(); // Remove the oldest reading
            }
            storage[i].push_back(reading);
        }

        outFile << "\nAverages: ";
        for (size_t i = 0; i < sensors->size(); ++i) {
            //for each sensor, print the running average of the last 10 readings
            float sum = 0;
            for (float r : storage[i]) {
                sum += r;
            }

            float average = sum / storage[i].size();
            outFile << std::setprecision(3) << std::setw(10) << std::fixed << average << " " << (*sensors)[i].getUnits() << " ";
        }

        outFile.close();
    }

    void saveLog(const time_t& currentTime, const std::string& filename) {
        std::ofstream outFile(filename, std::ios::app); // Open file in append mode
        if (!outFile.is_open()) {
            throw std::runtime_error("Failed to open file: " + filename);
        }

        outFile << std::put_time(std::localtime(&currentTime), "%Y-%m-%d %H:%M:%S") << ": ";
        for (size_t i = 0; i < sensors->size(); ++i) {
            float reading = (*sensors)[i](); // Get the current reading from the sensor
            outFile << std::setprecision(3) << std::setw(10) << std::fixed << reading << " " << (*sensors)[i].getUnits() << " ";
        }
        outFile << "\n";
        outFile.close();
    }


    std::string filePath;
    std::vector<std::deque<float>> storage;
    std::shared_ptr<std::vector<sensor>> sensors;
};