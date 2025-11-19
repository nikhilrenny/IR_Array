#ifndef IR_ARRAY_H
#define IR_ARRAY_H

#include <Arduino.h>
#include <vector>

class IR_Array {
public:
    // Constructor
    IR_Array(uint8_t count);

    // Setup (pin assignment + K value)
    void setup(const std::vector<uint8_t>& pins = {}, int k = 2);

    // Main functions
    void CalibrateSensors(uint16_t ctime = 5000);
    void Read(uint8_t mode = 0, uint8_t pd = 0);
    void DebugData();

    // Getters for sensor statistics
    const std::vector<float>& getMean() const { return Mean; }
    const std::vector<float>& getStdDev() const { return Std_Dev; }
    const std::vector<float>& getVariance() const { return Variance; }
    const std::vector<float>& getRange() const { return Range; }
    const std::vector<float>& getSensorStatus() const { return SensorStatus; }
    const std::vector<float>& getSum() const { return Sum; }
    const std::vector<uint16_t>& getCount() const { return Count; }
    const std::vector<float>& getSumSq() const { return Sum_Sq; }
    const std::vector<float>& getSensorMin() const { return SensorMin; }
    const std::vector<float>& getSensorMax() const { return SensorMax; }
    const std::vector<uint8_t>& getSensorPins() const { return SensorPins; }

private:
    // Sensor Properties
    const uint8_t SensorCount;       // Number of sensors
    std::vector<uint8_t> SensorPins; // Sensor pin array
    int K;                            // Sigma factor
    float Sum_Std_Dev;

    // Statistics Arrays
    std::vector<uint16_t> Count;
    std::vector<float> Sum;
    std::vector<float> Sum_Sq;
    std::vector<float> SensorMin;
    std::vector<float> SensorMax;
    std::vector<float> Range;
    std::vector<float> Mean;
    std::vector<float> Variance;
    std::vector<float> Std_Dev;
    std::vector<float> SensorStatus;
};

#endif
