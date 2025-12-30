#ifndef VANSIGHT_LIB_RELAY_H
#define VANSIGHT_LIB_RELAY_H

#include <Arduino.h>

class Relay {
public:
    Relay(uint8_t pin, bool isInverted = false);

    ~Relay();

    void on(void);

    void off(void);

    void toggle(void);

    bool isRelayOn(void);

private:
    uint8_t m_pin; /**< Output pin bound with this relay instance */

    /**
     * Signal levels as read from <code>digitalRead()</code> function, can be either <code>HIGH</code> or
     * <code>LOW</code>. When you create a new Relay instance, you specify <code>isInverted</code> boolean parameter.
     * This variable represents a mapping from the boolean to actual voltage level set on output.
     *
     * @see Relay(uint8_t pin, bool isInverted = true)
     */
    byte m_relayOnLogicLevel;

    /**
     * @brief States allowed for a Relay to transition into.
     */
    enum class State {
        RELAY_OFF,
        RELAY_ON
    };

    /**
     * This variable holds information on a current state of a relay.
     *
     * @see enum class State
     */
    State m_state = State::RELAY_OFF;
};

#endif // VANSIGHT_LIB_RELAY_H
