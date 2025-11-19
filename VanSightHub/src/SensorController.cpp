#include "SensorController.h"

SensorController::SensorController(uint8_t count) 
    : _count(count) {
    // Allocate array of LevelSensor pointers
    _sensors = new LevelSensor*[_count];
    
    // Initialize all pointers to nullptr
    for (uint8_t i = 0; i < _count; i++) {
        _sensors[i] = nullptr;
    }
}

SensorController::~SensorController() {
    // Clean up all sensor objects
    for (uint8_t i = 0; i < _count; i++) {
        if (_sensors[i] != nullptr) {
            delete _sensors[i];
        }
    }
    delete[] _sensors;
}

bool SensorController::addSensor(uint8_t index, uint8_t pin, float minResistance, float maxResistance, float referenceResistor) {
    if (index >= _count) return false;
    
    // Create new LevelSensor object
    _sensors[index] = new LevelSensor(pin, minResistance, maxResistance, referenceResistor);
    return true;
}

void SensorController::begin() {
    // Initialize all sensors
    for (uint8_t i = 0; i < _count; i++) {
        if (_sensors[i] != nullptr) {
            _sensors[i]->begin();
        }
    }
}

bool SensorController::isValidSensorNum(uint8_t sensorNum) const {
    return (sensorNum >= 1 && sensorNum <= _count && _sensors[sensorNum - 1] != nullptr);
}

float SensorController::readResistance(uint8_t sensorNum) {
    if (!isValidSensorNum(sensorNum)) return -1.0;
    
    return _sensors[sensorNum - 1]->readResistance();
}

int SensorController::readLevel(uint8_t sensorNum) {
    if (!isValidSensorNum(sensorNum)) return -1;
    
    return _sensors[sensorNum - 1]->readLevel();
}

float SensorController::readVoltage(uint8_t sensorNum) {
    if (!isValidSensorNum(sensorNum)) return -1.0;
    
    return _sensors[sensorNum - 1]->readVoltage();
}

int SensorController::readRaw(uint8_t sensorNum) {
    if (!isValidSensorNum(sensorNum)) return -1;
    
    return _sensors[sensorNum - 1]->readRaw();
}
