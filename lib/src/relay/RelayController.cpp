#include "RelayController.h"

#include "Relay.h"

RelayController::RelayController(const int* pins, int count) 
    : _count(count) {
    // Allocate array of SimpleRelay pointers
    _relays = new Relay*[_count];
    
    // Create SimpleRelay objects for each pin
    for (int i = 0; i < _count; i++) {
        _relays[i] = new Relay(pins[i], true);
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
    return (relayNum >= 0 && relayNum < _count);
}

bool RelayController::turnOn(int relayNum) {
    if (!isValidRelayNum(relayNum)) return false;
    
    _relays[relayNum]->on();
    return true;
}

bool RelayController::turnOff(int relayNum) {
    if (!isValidRelayNum(relayNum)) return false;
    
    _relays[relayNum]->off();
    return true;
}

bool RelayController::toggle(int relayNum) {
    if (!isValidRelayNum(relayNum)) return false;
    
    _relays[relayNum]->toggle();
    return true;
}

bool RelayController::getState(int relayNum) {
    if (!isValidRelayNum(relayNum)) return false;
    
    return _relays[relayNum]->isRelayOn();
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
