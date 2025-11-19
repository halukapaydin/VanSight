#ifndef SENSOR_CONTROLLER_H
#define SENSOR_CONTROLLER_H

#include <Arduino.h>
#include "LevelSensor.h"

/**
 * @brief SensorController class for managing multiple level sensors
 * 
 * This class provides a centralized interface for controlling
 * multiple level sensors with simple methods.
 */
class SensorController {
public:
    /**
     * @brief Construct a new Sensor Controller object
     * 
     * @param count Number of sensors
     */
    SensorController(uint8_t count);
    
    /**
     * @brief Destroy the Sensor Controller object
     */
    ~SensorController();
    
    /**
     * @brief Add a sensor to the controller
     * 
     * @param index Sensor index (0-based)
     * @param pin ADC pin number
     * @param minResistance Minimum resistance in ohms
     * @param maxResistance Maximum resistance in ohms
     * @param referenceResistor Reference resistor value (default: 1000Ω)
     * @return true if successful, false if invalid index
     */
    bool addSensor(uint8_t index, uint8_t pin, float minResistance, float maxResistance, float referenceResistor = 1000.0);
    
    /**
     * @brief Initialize all sensors
     */
    void begin();
    
    /**
     * @brief Read resistance from a specific sensor
     * 
     * @param sensorNum Sensor number (1-based index)
     * @return float Resistance in ohms, -1 if invalid sensor
     */
    float readResistance(uint8_t sensorNum);
    
    /**
     * @brief Read level from a specific sensor
     * 
     * @param sensorNum Sensor number (1-based index)
     * @return int Level percentage (0-100), -1 if invalid sensor
     */
    int readLevel(uint8_t sensorNum);
    
    /**
     * @brief Read voltage from a specific sensor
     * 
     * @param sensorNum Sensor number (1-based index)
     * @return float Voltage in volts, -1 if invalid sensor
     */
    float readVoltage(uint8_t sensorNum);
    
    /**
     * @brief Read raw ADC value from a specific sensor
     * 
     * @param sensorNum Sensor number (1-based index)
     * @return int ADC value (0-4095), -1 if invalid sensor
     */
    int readRaw(uint8_t sensorNum);
    
    /**
     * @brief Get the total number of sensors
     * 
     * @return uint8_t Number of sensors
     */
    uint8_t getCount() const { return _count; }

private:
    LevelSensor** _sensors;
    uint8_t _count;
    
    /**
     * @brief Validate sensor number
     * 
     * @param sensorNum Sensor number (1-based)
     * @return true if valid, false otherwise
     */
    bool isValidSensorNum(uint8_t sensorNum) const;
};

#endif // SENSOR_CONTROLLER_H
