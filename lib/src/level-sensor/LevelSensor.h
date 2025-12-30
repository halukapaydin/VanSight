#ifndef LEVEL_SENSOR_H
#define LEVEL_SENSOR_H

#include <Arduino.h>

/**
 * @brief LevelSensor class for reading resistance-based level sensors
 * 
 * This class provides an object-oriented interface for reading analog
 * resistance-based level sensors using voltage divider circuits.
 */
class LevelSensor {
public:
    /**
     * @brief Construct a new Level Sensor object
     * 
     * @param pin Analog pin number (e.g., A0, A1)
     * @param minResistance Minimum resistance value in ohms
     * @param maxResistance Maximum resistance value in ohms
     * @param referenceResistor Reference resistor value in voltage divider (default: 1000Ω)
     */
    LevelSensor(uint8_t pin, float minResistance, float maxResistance, float referenceResistor, float adcResolution, float supplyVoltage);
    
    /**
     * @brief Destroy the Level Sensor object
     */
    ~LevelSensor();
    
    /**
     * @brief Initialize the sensor (configure pin)
     */
    void begin();
    
    /**
     * @brief Read the current resistance value
     * 
     * @return float Resistance in ohms
     */
    float readResistance();
    
    /**
     * @brief Read the level as a percentage (0-100%)
     * 
     * @return int Level percentage (0-100)
     */
    int readLevel();
    
    /**
     * @brief Get the raw ADC value
     * 
     * @return int ADC value (0-1023 for 10-bit)
     */
    int readRaw();
    
    /**
     * @brief Get the voltage at the ADC pin
     * 
     * @return float Voltage in volts
     */
    float readVoltage();

private:
    uint8_t _pin;
    float _minResistance;
    float _maxResistance;
    float _referenceResistor;
    float _adc_resolution;
    float _supply_voltage;
};

#endif // LEVEL_SENSOR_H
