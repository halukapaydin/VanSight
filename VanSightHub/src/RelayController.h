#ifndef RELAY_CONTROLLER_H
#define RELAY_CONTROLLER_H

#include <Arduino.h>
#include <SimpleRelay.h>

/**
 * @brief RelayController class for managing multiple relays
 * 
 * This class provides a centralized interface for controlling
 * multiple relays with simple methods.
 */
class RelayController {
public:
    /**
     * @brief Construct a new Relay Controller object
     * 
     * @param pins Array of GPIO pins for relays
     * @param count Number of relays
     */
    RelayController(const int* pins, int count);
    
    /**
     * @brief Destroy the Relay Controller object
     */
    ~RelayController();
    
    /**
     * @brief Initialize all relays
     */
    void begin();
    
    /**
     * @brief Turn on a specific relay
     * 
     * @param relayNum Relay number (1-based index)
     * @return true if successful, false if invalid relay number
     */
    bool turnOn(int relayNum);
    
    /**
     * @brief Turn off a specific relay
     * 
     * @param relayNum Relay number (1-based index)
     * @return true if successful, false if invalid relay number
     */
    bool turnOff(int relayNum);
    
    /**
     * @brief Toggle a specific relay
     * 
     * @param relayNum Relay number (1-based index)
     * @return true if successful, false if invalid relay number
     */
    bool toggle(int relayNum);
    
    /**
     * @brief Get the state of a specific relay
     * 
     * @param relayNum Relay number (1-based index)
     * @return true if ON, false if OFF or invalid relay number
     */
    bool getState(int relayNum);
    
    /**
     * @brief Turn on all relays
     */
    void allOn();
    
    /**
     * @brief Turn off all relays
     */
    void allOff();
    
    /**
     * @brief Get the total number of relays
     * 
     * @return uint Number of relays
     */
    uint getCount() const { return _count; }

private:
    SimpleRelay** _relays;
    int _count;
    
    /**
     * @brief Validate relay number
     * 
     * @param relayNum Relay number (1-based)
     * @return true if valid, false otherwise
     */
    bool isValidRelayNum(int relayNum) const;
};

#endif // RELAY_CONTROLLER_H
