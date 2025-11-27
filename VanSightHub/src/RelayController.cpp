#include "RelayController.h"

RelayController::RelayController(const int* pins, int count) 
    : _count(count) {
    // Allocate array of SimpleRelay pointers
    _relays = new SimpleRelay*[_count];
    
    // Create SimpleRelay objects for each pin
    for (int i = 0; i < _count; i++) {
        _relays[i] = new SimpleRelay(pins[i]);
    }
}

RelayController::~RelayController() {
    // Clean up all relay objects
    for (int i = 0; i < _count; i++) {
        delete _relays[i];
    }
    delete[] _relays;
}

void RelayController::begin() {
    // SimpleRelay handles initialization in constructor
    // Nothing additional needed here
}

bool RelayController::isValidRelayNum(int relayNum) const {
    return (relayNum >= 1 && relayNum <= _count);
}

bool RelayController::turnOn(int relayNum) {
    if (!isValidRelayNum(relayNum)) return false;
    
    _relays[relayNum - 1]->on();
    return true;
}

bool RelayController::turnOff(int relayNum) {
    if (!isValidRelayNum(relayNum)) return false;
    
    _relays[relayNum - 1]->off();
    return true;
}

bool RelayController::toggle(int relayNum) {
    if (!isValidRelayNum(relayNum)) return false;
    
    _relays[relayNum - 1]->toggle();
    return true;
}

bool RelayController::getState(int relayNum) {
    if (!isValidRelayNum(relayNum)) return false;
    
    return _relays[relayNum - 1]->isRelayOn();
}

void RelayController::allOn() {
    for (int i = 0; i < _count; i++) {
        _relays[i]->on();
    }
}

void RelayController::allOff() {
    for (int i = 0; i < _count; i++) {
        _relays[i]->off();
    }
}
