#include "SensorController.h"

SensorController::SensorController(int count) 
    : _count(count) {
    // Allocate array of LevelSensor pointers
    _sensors = new LevelSensor*[_count];
    
    // Initialize all pointers to nullptr
    for (int i = 0; i < _count; i++) {
        _sensors[i] = nullptr;
    }
}

SensorController::~SensorController() {
    // Clean up all sensor objects
    for (int i = 0; i < _count; i++) {
        if (_sensors[i] != nullptr) {
            delete _sensors[i];
        }
    }
    delete[] _sensors;
}

bool SensorController::addSensor(int index, int pin, float minResistance, float maxResistance, float referenceResistor) {
    if (index >= _count) return false;
    
    // Create new LevelSensor object
    _sensors[index] = new LevelSensor(pin, minResistance, maxResistance, referenceResistor);
    return true;
}

void SensorController::begin() {
    // Initialize all sensors
    for (int i = 0; i < _count; i++) {
        if (_sensors[i] != nullptr) {
            _sensors[i]->begin();
        }
    }
}

bool SensorController::isValidSensorNum(int sensorNum) const {
    return (sensorNum >= 1 && sensorNum <= _count && _sensors[sensorNum - 1] != nullptr);
}

float SensorController::readResistance(int sensorNum) {
    if (!isValidSensorNum(sensorNum)) return -1.0;
    
    return _sensors[sensorNum - 1]->readResistance();
}

int SensorController::readLevel(int sensorNum) {
    if (!isValidSensorNum(sensorNum)) return -1;
    
    return _sensors[sensorNum - 1]->readLevel();
}

float SensorController::readVoltage(int sensorNum) {
    if (!isValidSensorNum(sensorNum)) return -1.0;
    
    return _sensors[sensorNum - 1]->readVoltage();
}

int SensorController::readRaw(int sensorNum) {
    if (!isValidSensorNum(sensorNum)) return -1;
    
    return _sensors[sensorNum - 1]->readRaw();
}
