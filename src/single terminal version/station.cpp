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
    WeatherStation(std::shared_ptr<std::vector<sensor>> inSensors): storage(inSensors->size()), sensors(inSensors) {}

    void run(std::mutex& terminalMutex,time_t currentTime) {
        time_t nextTime = currentTime + 1;
        while (true) {
            //lock the terminal for this thread
            while(!terminalMutex.try_lock()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(10)); // Wait for a bit before trying again
            }

            print(currentTime);
            terminalMutex.unlock();

            //sleep until next time
            std::this_thread::sleep_until(std::chrono::system_clock::from_time_t(nextTime));

            currentTime = nextTime;
            nextTime = nextTime + 1;
        }
    }

private:
    void print(const time_t& currentTime){
        resetTerminal();

        std::cout << "Weather Station Data\n";

        std::cout << "Time: " << std::put_time(std::localtime(&currentTime), "%Y-%m-%d %H:%M:%S") << "\n";

        std::cout << "Readings: ";
        for (size_t i = 0; i < sensors->size(); ++i) {
            float reading = (*sensors)[i](); // Get the current reading from the sensor
            std::cout << std::setprecision(3) << std::setw(10) << reading << " " << (*sensors)[i].getUnits() << " ";

            // Update the deque with the new reading
            if (storage[i].size() == 10) {
                storage[i].pop_front(); // Remove the oldest reading
            }
            storage[i].push_back(reading);
        }

        std::cout << "\nAverages: ";
        for (size_t i = 0; i < sensors->size(); ++i) {
            //for each sensor, print the running average of the last 10 readings
            float sum = 0;
            for (float r : storage[i]) {
                sum += r;
            }

            float average = sum / storage[i].size();
            std::cout << std::setprecision(3) << std::setw(10) << average << " " << (*sensors)[i].getUnits() << " ";
        }

        std::cout << "\033[" << 7 << ";1H"; // ANSI: Move cursor to line n, column 1
    }

    void resetTerminal(){
        for(int n = 4; n > 1; n--){
            std::cout << "\033[" << n << ";1H"; // ANSI: Move cursor to line n, column 1
            std::cout << "\033[2K"; // ANSI: Clear entire line
        }
    }

    std::string filePath;
    std::vector<std::deque<float>> storage;
    std::shared_ptr<std::vector<sensor>> sensors;
};