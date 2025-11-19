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
