#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include "RelayController.h"
#include "SensorController.h"

/**
 * @brief CommandHandler class for processing ESP-NOW commands
 * 
 * This class handles all incoming commands and generates appropriate responses.
 */
class CommandHandler {
public:
    /**
     * @brief Construct a new Command Handler object
     * 
     * @param relayCtrl Reference to RelayController
     * @param sensorCtrl Reference to SensorController
     */
    CommandHandler(RelayController& relayCtrl, SensorController& sensorCtrl);
    
    /**
     * @brief Process a command and generate response
     * 
     * @param data Command data (JSON)
     * @param len Data length
     * @param response Response document to populate
     * @return true if command was valid, false otherwise
     */
    bool processCommand(const uint8_t* data, int len, JsonDocument& response);

private:
    RelayController& _relayController;
    SensorController& _sensorController;
    
    // Command handlers
    void handleRelayOn(JsonDocument& doc, JsonDocument& response);
    void handleRelayOff(JsonDocument& doc, JsonDocument& response);
    void handleRelayToggle(JsonDocument& doc, JsonDocument& response);
    void handleRelayStatus(JsonDocument& doc, JsonDocument& response);
    void handleAllRelaysOn(JsonDocument& response);
    void handleAllRelaysOff(JsonDocument& response);
    void handleAllRelayStatus(JsonDocument& response);
    void handleSensorStatus(JsonDocument& doc, JsonDocument& response);
    void handleAllSensorStatus(JsonDocument& response);
    void handleAllStatus(JsonDocument& response);
    
    // Helper methods
    void sendSuccess(JsonDocument& response, JsonDocument& data, const char* message = "");
    void sendError(JsonDocument& response, const char* message);
};

#endif // COMMAND_HANDLER_H
