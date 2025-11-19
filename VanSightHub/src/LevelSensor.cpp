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
    float voltage = readVoltage();
    
    // Prevent division by zero
    if (voltage >= SUPPLY_VOLTAGE - 0.01) {
        return _maxResistance;
    }
    
    // Calculate resistance using voltage divider formula
    // Vout = Vin * (R_sensor / (R_ref + R_sensor))
    // Solving for R_sensor:
    // R_sensor = (Vout * R_ref) / (Vin - Vout)
    float resistance = (voltage * _referenceResistor) / (SUPPLY_VOLTAGE - voltage);
    
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
