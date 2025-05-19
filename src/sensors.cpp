#pragma once

//defines objects that read some measurements from ambient conditions
#include <random>
#include <memory>
#include <type_traits>

class sensor{
    public:
        //simulates a sensor with a minimum possible reading, a maximum possible reading, a specified precision tolerance (e.g. ±1°C),
        sensor(float min, float max, float precision, std::shared_ptr<float> ambientCondition, std::string&& units){
            this->ambientConditionPtr = ambientCondition;

            this->min = min;
            this->max = max;

            //99.7% of readings are within the 
            this->stddev = precision / 3.0; 
            this->precision = precision;

            this->units = std::move(units);

            this->generator.seed(std::random_device{}());
        }

        //overloads the () operator to return the reading of the sensor
        float operator()(){

            //create a shared pointer to access the object
            if (!ambientConditionPtr) {
                throw std::runtime_error("Ambient condition object has expired.");
            }

            //get a reading with some error
            float reading = truncatedNormalDistribution(*ambientConditionPtr, stddev, precision);

            // Ensure the reading is within the range of the sensor
            if (reading < min) {
                reading = min;
            } else if (reading > max) {
                reading = max;
            }

            return reading;
        }

        const std::string& getUnits() const {
            return units;
        }

        float getActual(){
            if (!ambientConditionPtr) {
                throw std::runtime_error("Ambient condition object has expired.");
            }
            return *ambientConditionPtr;
        }

    protected:
        std::shared_ptr<float> ambientConditionPtr;
        float min, max, stddev, precision;

        std::string units;

        std::mt19937 generator;

        //reads from a normal distribution around the true ambient condition, but ensures that the reading is not outside the specified precision
        float truncatedNormalDistribution(float mean, float stddev, float precision) {
            float minTolerance = mean - precision;
            float maxTolerance = mean + precision;

            //create a normal distribution
            std::normal_distribution<float> distribution(mean, stddev);

            float reading;
            do{ //keep generating readings until we get a value within the precision range
                reading = distribution(generator);
            } while( reading < minTolerance || reading > maxTolerance);

            return reading;
        }
};

