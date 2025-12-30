#include "LevelSensor.h"
#include "config.h"

LevelSensor::LevelSensor(uint8_t pin, float minResistance, float maxResistance, float referenceResistor, float adcResolution, float supplyVoltage)
    : _pin(pin),
      _minResistance(minResistance),
      _maxResistance(maxResistance),
      _referenceResistor(referenceResistor),
      _adc_resolution(adcResolution),
      _supply_voltage(supplyVoltage)
{
}

LevelSensor::~LevelSensor()
{
    // Nothing to clean up
}

void LevelSensor::begin()
{
    pinMode(_pin, INPUT);
}

int LevelSensor::readRaw()
{
    return analogRead(_pin);
}

float LevelSensor::readVoltage()
{
    int adcValue = readRaw();
    // Use config.h constants for adaptability
    return (adcValue / (float)_adc_resolution) * _supply_voltage;
}

float LevelSensor::readResistance()
{
    int adcValue = readRaw();
    float voltage = readVoltage();

    // Prevent division by zero
    if (voltage >= _supply_voltage - 0.01)
    {
        return _maxResistance;
    }

    float resistance = 0.0;

    // If no reference resistor (direct potentiometer connection)
    if (_referenceResistor < 1.0)
    {
        // Direct voltage to resistance mapping for potentiometer
        float ratio = voltage / _supply_voltage;
        resistance = ratio * 10000.0; // 10K pot assumption
    }
    else
    {
        // Calculate resistance using voltage divider formula
        // Vout = Vin * (R_sensor / (R_ref + R_sensor))
        // R_sensor = (Vout * R_ref) / (Vin - Vout)
        resistance = (voltage * _referenceResistor) / (_supply_voltage - voltage);
    }

    return resistance;
}

int LevelSensor::readLevel()
{
    float resistance = readResistance();

    // Convert resistance to percentage (0-100%)
    float level = ((resistance - _minResistance) / (_maxResistance - _minResistance)) * 100.0;

    // Clamp to 0-100 range
    if (level < 0) level = 0;
    if (level > 100) level = 100;

    return (int)level;
}
