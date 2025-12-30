
#include "Relay.h"

Relay::Relay(uint8_t pin, bool isInverted) {
    m_pin = pin;
    m_relayOnLogicLevel = isInverted ? LOW : HIGH;
    pinMode(m_pin, OUTPUT);
    off();
}

Relay::~Relay() {
    // set pin to high impedance
    pinMode(m_pin, INPUT);
}

void Relay::on(void) {
    m_state = State::RELAY_ON;
    digitalWrite(m_pin, m_relayOnLogicLevel);
}

void Relay::off(void) {
    m_state = State::RELAY_OFF;
    digitalWrite(m_pin, m_relayOnLogicLevel == HIGH ? LOW : HIGH);
}

void Relay::toggle(void) {
    if (m_state == State::RELAY_ON) {
        off();
    } else {
        on();
    }
}

bool Relay::isRelayOn(void) {
    return m_state == State::RELAY_ON;
}
