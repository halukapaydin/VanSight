#include "LevelSensor.h"

LevelSensor::LevelSensor(uint8_t pin, float minResistance, float maxResistance, float referenceResistor)
    : _pin(pin), 
      _minResistance(minResistance), 
      _maxResistance(maxResistance),
      _referenceResistor(referenceResistor) {
}

LevelSensor::~LevelSensor() {
    // Nothing to clean up
}

void LevelSensor::begin() {
    pinMode(_pin, INPUT);
    // ADC configuration is done globally in main.cpp
}

int LevelSensor::readRaw() {
    return analogRead(_pin);
}

float LevelSensor::readVoltage() {
    int adcValue = readRaw();
    return (adcValue / (float)ADC_RESOLUTION) * SUPPLY_VOLTAGE;
}

float LevelSensor::readResistance() {
    int adcValue = readRaw();
    float voltage = readVoltage();
    
    // DEBUG: Print raw values
    Serial.printf("[Sensor Pin %d] ADC Raw: %d, Voltage: %.3fV, Ref: %.1fΩ\n", 
                  _pin, adcValue, voltage, _referenceResistor);
    
    // Prevent division by zero
    if (voltage >= SUPPLY_VOLTAGE - 0.01) {
        return _maxResistance;
    }
    
    float resistance = 0.0;
    
    // If no reference resistor (direct potentiometer connection)
    if (_referenceResistor < 1.0) {
        // Direct voltage to resistance mapping for potentiometer
        // Assuming pot is connected: 3.3V -- Pot -- ADC -- GND
        float ratio = voltage / SUPPLY_VOLTAGE;
        resistance = ratio * 10000.0; // 10K pot
        Serial.printf("  -> Direct mode: Raw Resistance = %.1fΩ\n", resistance);
    } else {
        // Calculate resistance using voltage divider formula
        // Vout = Vin * (R_sensor / (R_ref + R_sensor))
        // Solving for R_sensor:
        // R_sensor = (Vout * R_ref) / (Vin - Vout)
        resistance = (voltage * _referenceResistor) / (SUPPLY_VOLTAGE - voltage);
        Serial.printf("  -> Voltage divider mode: Raw Resistance = %.1fΩ\n", resistance);
    }
    
    // Apply calibration factor to correct for ADC non-linearity and component tolerances
    // Calibration: Measured 9360Ω when actual is 10000Ω -> factor = 10000/9360 = 1.068
    const float CALIBRATION_FACTOR = 1.068;
    resistance = resistance * CALIBRATION_FACTOR;
    
    Serial.printf("  -> Calibrated Resistance = %.1fΩ\n", resistance);
    return resistance;
}

int LevelSensor::readLevel() {
    float resistance = readResistance();
    
    // Convert resistance to percentage (0-100%)
    float level = ((resistance - _minResistance) / (_maxResistance - _minResistance)) * 100.0;
    
    // Clamp to 0-100 range
    if (level < 0) level = 0;
    if (level > 100) level = 100;
    
    return (int)level;
}
