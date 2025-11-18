# IR_Array Library

![Version](https://img.shields.io/badge/version-1.0.0-blue) ![License](https://img.shields.io/badge/license-MIT-green) ![Platform](https://img.shields.io/badge/platform-Arduino-red)

**Author:** Nikhil Renny

---

The **IR_Array** library enables efficient interfacing with multiple IR sensors. Designed primarily for Arduino, it also supports PlatformIO, ESP32/ESP8266, Teensy, and other platforms. Lightweight and scalable, it is ideal for projects requiring any number of IR sensors.

---

## Features

- Automatic calibration of IR sensors  
- Raw, filtered, and normalized readings  
- Sensor health monitoring  
- Detect the position of active sensors (Left, Center, Right)  
- Easy debugging using `DebugData()` function  
- Configurable number of sensors and pin assignments  

---

## Installation

1. Clone the repository:

```bash
git clone git@github.com:nikhilrenny/IR_Array.git
```
2. Include the library in your Arduino sketch:
 ```cpp
#include <IR_Array.h>
```
The recommended method to install the **IR_Array** library is via the Arduino IDE Library Manager or through a package manager such as PlatformIO.
>
> Alternatively, you can manually install the library by downloading it and extracting the files into a folder named `IR_Array` within your Arduino custom libraries directory, typically located at:
>
> * **Windows:** `Documents\Arduino\libraries`
> * **Linux/macOS:** `~/Documents/Arduino/libraries/`

---
## Library Initialization

The `setup()` function initializes the IR sensor array and configures the **sigma (K)** value used for sensor health checks:

1. **Sigma Value (**``**):**

   * Sets the `K` parameter, which defines the tolerance range around the mean (mean ± K·std_dev) for assessing sensor health.
2. **Sensor Pin Assignment (**``**):**

   * If a valid array of pins is provided, it assigns each sensor to the corresponding pin.
   * If `pins` is `nullptr`, it defaults to **consecutive analog pins** starting from `A0`.

This function must be called **before calibration or reading** to ensure the sensor array is correctly configured.

### Pin Settings

* **Default pins:** A0 to A7 for 8 sensors
* **Custom pins:** Pass an array to `setup()`

#### Example:

Sigma (K) sets the tolerance range for sensor health based on a Gaussian distribution.

```cpp
ir.setup();  // deafault A0 to A7

uint8_t pins8[8] = {A0, A1, A2, A3, A4, A5, A6, A7};
IR_Array ir8(8);
ir8.setup(pins8);       // default sigma value = 2
ir8.setup(pins8, 2);    // optional  sigma value 
```
---

## Calibration

### Sensor Calibration Process

1. **Data Collection:** Over the default period of **5000 ms** (configurable via `ctime`), each sensor’s analog values are repeatedly read. The library records the **sum**, **sum of squares**, **maximum**, and **minimum** for each sensor.
2. **Statistical Analysis:** From the collected data, it computes the **mean**, **variance**, **standard deviation**, and **range** for each sensor. This creates a statistical baseline of normal readings.
3. **Sensor Health Check:** Using a **Gaussian distribution**, each sensor’s current reading is compared against **mean ± K·standard deviation**:

   * Within range → healthy (`1`)
   * Slightly outside → warning (`0.5`)
   * Far outside → unhealthy (`0`)
4. **Completion:** Calibration finalizes the baseline values, enabling reliable detection and health monitoring during operation.

```cpp
ir.CalibrateSensors();        // Default: 5000 ms
ir.CalibrateSensors(5000);    // Custom duration in milliseconds
```

---

## Reading Sensor Data

### IR Sensor Reading Modes

The `Read()` function provides multiple ways to process and interpret the IR sensor data, with optional serial output controlled by `pd`:

1. **Raw Data (mode = 1):**

   * Reads each sensor’s current analog value without modification.
   * If `pd = 1`, the values are printed to the serial monitor; if `pd = 0`, no output is printed.
2. **Filtered Data (mode = 2):**

   * Normalizes readings using each sensor’s **mean** and **standard deviation**:

     NormalizedValue=SensorValue−MeanStd_Dev\text{NormalizedValue} = \frac{\text{SensorValue} - \text{Mean}}{\text{Std\_Dev}}NormalizedValue=Std_DevSensorValue−Mean
   * `pd` controls whether these filtered values are printed.
3. **Normalized Data (mode = 3):**

   * Performs **dynamic normalization**: readings are mapped to `0` or `1` based on their deviation from the mean.
   * `pd` determines whether the binary values are printed.
4. **Position / Default Mode (any other mode):**

   * Uses normalized z-scores to identify active sensors.
   * Computes the **average active index** to estimate the object’s **position**: `"Left"`, `"Center"`, or `"Right"`.
   * If `pd = 1`, the position and center value are printed; if `pd = 0`, no output is generated.

```cpp
ir.Read(mode, printFlag);
```

* **mode:**

| Mode        | Description                                               |
| ----------- | --------------------------------------------------------- |
| 0 (default) | Detects direction of active sensors (Left, Center, Right) |
| 1           | Raw sensor readings                                       |
| 2           | Filtered sensor data (z-score)                            |
| 3           | Normalized sensor readings (0 or 1)                       |

* **printFlag:**

| Value | Description            |
| ----- | ---------------------- |
| 0     | Do not print           |
| 1     | Print values to Serial |

### Example:

```cpp
ir.Read(1, 1); // Raw data, print enabled
ir.Read(2, 1); // Filtered data
ir.Read(3, 1); // Normalized data
ir.Read();     // Default mode (direction)
```

---

## Debug Data

### Sensor Data Debug Summary

The `DebugData()` function prints a detailed summary of all sensors to the serial monitor, allowing you to inspect calibration and current status. It provides the following information for each sensor:

1. **Index:** Sensor number in the array.
2. **Count:** Number of samples collected during calibration.
3. **Min / Max:** Minimum and maximum readings recorded.
4. **Range:** Difference between maximum and minimum values.
5. **Mean:** Average reading from calibration.
6. **StdDev:** Standard deviation of the readings, indicating variability.
7. **Status:** Health of the sensor (`1` = healthy, `0.5` = warning, `0` = unhealthy).

At the end, it prints the **sum of all standard deviations** (`Sum_Std_Dev`) to give an overall sense of sensor variability.

This function is useful for **debugging**, verifying calibration, and checking the health of each IR sensor in the array.

```cpp
ir.DebugData();  // Prints summary for all sensors
```

---

## Baud Rate Calculation

To ensure stable readings from multiple sensors, it is important to select a sufficient baud rate for serial communication.

### Estimating Required Baud Rate

**Assumptions:**

* Number of sensors: `N = 8`
* Each analog reading is transmitted as ASCII via `Serial.print`, approximately 6–8 characters per reading plus separators (`|`, newline). Assume **10 bytes per reading**.
* Sampling rate per sensor: `f_s = 1 kHz` (typical for fast calibration)

### Data per second

| Parameter           | Value                                                          |
| ------------------- | -------------------------------------------------------------- |
| Number of sensors N | 8                                                              |
| Bytes per reading   | 10                                                             |
| Sampling rate f_s   | 1,000 Hz                                                       |
| DataRate            | `N × BytesPerReading × f_s = 8 × 10 × 1000 = 80,000 bytes/sec` |
| Bits per second     | `DataRate × 8 = 80,000 × 8 = 640,000 bps`                      |

### Conclusion

* A baud rate of **500,000 bps** is close but slightly below the theoretical requirement for continuous streaming.
* Using **≥500,000–1,000,000 bps** ensures all data is transmitted reliably, accounting for small overheads.

Set `Serial.begin(500000)` or higher for reliable readings.

---

## Examples

```cpp
#include <IR_Array.h>

uint8_t pins[4] = {A0, A1, A2, A3};
IR_Array ir(4);

void setup() {
  Serial.begin(19200);
  ir.setup(pins);
  ir.CalibrateSensors();
}

void loop() {
  ir.Read(0, 1);  // Detect direction and print to Serial
  delay(100);
}
```

## License

This project is licensed under the **MIT License** – see the [LICENSE](LICENSE) file for details.
