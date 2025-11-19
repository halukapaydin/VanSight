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
    RelayController(const int* pins, uint8_t count);
    
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
    bool turnOn(uint8_t relayNum);
    
    /**
     * @brief Turn off a specific relay
     * 
     * @param relayNum Relay number (1-based index)
     * @return true if successful, false if invalid relay number
     */
    bool turnOff(uint8_t relayNum);
    
    /**
     * @brief Toggle a specific relay
     * 
     * @param relayNum Relay number (1-based index)
     * @return true if successful, false if invalid relay number
     */
    bool toggle(uint8_t relayNum);
    
    /**
     * @brief Get the state of a specific relay
     * 
     * @param relayNum Relay number (1-based index)
     * @return true if ON, false if OFF or invalid relay number
     */
    bool getState(uint8_t relayNum);
    
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
     * @return uint8_t Number of relays
     */
    uint8_t getCount() const { return _count; }

private:
    SimpleRelay** _relays;
    uint8_t _count;
    
    /**
     * @brief Validate relay number
     * 
     * @param relayNum Relay number (1-based)
     * @return true if valid, false otherwise
     */
    bool isValidRelayNum(uint8_t relayNum) const;
};

#endif // RELAY_CONTROLLER_H
