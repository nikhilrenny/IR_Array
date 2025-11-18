#ifndef IR_ARRAY_H
#define IR_ARRAY_H

#include <Arduino.h>

class IR_Array {
public:
    // Constructor
    IR_Array(uint8_t count);
    // Setup (pin assignment + K value)
    void setup(const uint8_t* pins = nullptr, int k = 2);
    // Main functions
    void CalibrateSensors(uint16_t ctime = 5000);
    void Read(uint8_t mode = 0, uint8_t pd = 0);
    void DebugData();

private:
    // Sensor Properties
    const uint8_t SensorCount;   // Number of sensors
    uint8_t*  SensorPins;        // Sensor pin array
    int       K;                 // Sigma factor
    float     Sum_Std_Dev;

    // Statistics Arrays
    uint16_t* Count;
    float*    Sum;
    float*    Sum_Sq;
    float*    SensorMin;
    float*    SensorMax;
    float*    Range;
    float*    Mean;
    float*    Variance;
    float*    Std_Dev;
    float*    SensorStatus;
};
#endif
