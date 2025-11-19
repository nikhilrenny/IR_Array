#ifndef IR_ARRAY_H
#define IR_ARRAY_H

#include <Arduino.h>

class IR_Array {
public:
    IR_Array(uint8_t count);
    void setup(const uint8_t* pins = nullptr, int k = 2);  // Setup (pin assignment + K value)
    
    void CalibrateSensors(uint16_t ctime = 5000); // Main functions
    void Read(uint8_t mode = 0, uint8_t pd = 0);
    void DebugData();

    // Getters for sensor statistics
    const float* getMean() const { return Mean; }
    const float* getStdDev() const { return Std_Dev; }
    const float* getVariance() const { return Variance; }
    const float* getRange() const { return Range; }
    const float* getSensorStatus() const { return SensorStatus; }
    const float* getSum() const { return Sum; }
    const uint16_t* getCount() const { return Count; }
    const float* getSumSq() const { return Sum_Sq; }
    const float* getSensorMin() const { return SensorMin; }
    const float* getSensorMax() const { return SensorMax; }
    const uint8_t* getSensorPins() const { return SensorPins; }

private:
    // Sensor Properties
    const uint8_t SensorCount;   // Number of sensors
    uint8_t* SensorPins;         // Sensor pin array
    int K;                        // Sigma factor
    float Sum_Std_Dev;

    // Statistics Arrays
    uint16_t* Count;
    float* Sum;
    float* Sum_Sq;
    float* SensorMin;
    float* SensorMax;
    float* Range;
    float* Mean;
    float* Variance;
    float* Std_Dev;
    float* SensorStatus;
};

#endif
