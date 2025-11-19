#include "CommandHandler.h"

CommandHandler::CommandHandler(RelayController& relayCtrl, SensorController& sensorCtrl)
    : _relayController(relayCtrl), _sensorController(sensorCtrl) {
}

bool CommandHandler::processCommand(const uint8_t* data, int len, JsonDocument& response) {
    // Parse JSON command
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, data, len);
    
    if (error) {
        Serial.printf("JSON parse error: %s\n", error.c_str());
        sendError(response, "Invalid JSON format");
        return false;
    }
    
    const char* cmd = doc["cmd"];
    if (!cmd) {
        sendError(response, "Missing 'cmd' field");
        return false;
    }
    
    // Log command
    Serial.printf("Command: %s", cmd);
    if (doc.containsKey("relay")) Serial.printf(" (Relay %d)", (int)doc["relay"]);
    if (doc.containsKey("sensor")) Serial.printf(" (Sensor %d)", (int)doc["sensor"]);
    Serial.println();
    
    // Route to appropriate handler
    if (strcmp(cmd, "relay_on") == 0) {
        handleRelayOn(doc, response);
    }
    else if (strcmp(cmd, "relay_off") == 0) {
        handleRelayOff(doc, response);
    }
    else if (strcmp(cmd, "relay_toggle") == 0) {
        handleRelayToggle(doc, response);
    }
    else if (strcmp(cmd, "relay_status") == 0) {
        handleRelayStatus(doc, response);
    }
    else if (strcmp(cmd, "all_relays_on") == 0) {
        handleAllRelaysOn(response);
    }
    else if (strcmp(cmd, "all_relays_off") == 0) {
        handleAllRelaysOff(response);
    }
    else if (strcmp(cmd, "all_relay_status") == 0) {
        handleAllRelayStatus(response);
    }
    else if (strcmp(cmd, "sensor_status") == 0) {
        handleSensorStatus(doc, response);
    }
    else if (strcmp(cmd, "all_sensor_status") == 0) {
        handleAllSensorStatus(response);
    }
    else if (strcmp(cmd, "all_status") == 0) {
        handleAllStatus(response);
    }
    else {
        sendError(response, "Unknown command");
        return false;
    }
    
    return true;
}

// ============================================================================
// RELAY COMMAND HANDLERS
// ============================================================================

void CommandHandler::handleRelayOn(JsonDocument& doc, JsonDocument& response) {
    int relayNum = doc["relay"] | 0;
    
    if (_relayController.turnOn(relayNum)) {
        JsonDocument data;
        data["relay"] = relayNum;
        data["state"] = "on";
        sendSuccess(response, data, "Relay turned ON");
        Serial.printf("Relay %d: ON\n", relayNum);
    } else {
        sendError(response, "Invalid relay number (1-16)");
    }
}

void CommandHandler::handleRelayOff(JsonDocument& doc, JsonDocument& response) {
    int relayNum = doc["relay"] | 0;
    
    if (_relayController.turnOff(relayNum)) {
        JsonDocument data;
        data["relay"] = relayNum;
        data["state"] = "off";
        sendSuccess(response, data, "Relay turned OFF");
        Serial.printf("Relay %d: OFF\n", relayNum);
    } else {
        sendError(response, "Invalid relay number (1-16)");
    }
}

void CommandHandler::handleRelayToggle(JsonDocument& doc, JsonDocument& response) {
    int relayNum = doc["relay"] | 0;
    
    if (_relayController.toggle(relayNum)) {
        bool state = _relayController.getState(relayNum);
        JsonDocument data;
        data["relay"] = relayNum;
        data["state"] = state ? "on" : "off";
        sendSuccess(response, data, "Relay toggled");
        Serial.printf("Relay %d toggled: %s\n", relayNum, state ? "ON" : "OFF");
    } else {
        sendError(response, "Invalid relay number (1-16)");
    }
}

void CommandHandler::handleRelayStatus(JsonDocument& doc, JsonDocument& response) {
    int relayNum = doc["relay"] | 0;
    
    if (relayNum >= 1 && relayNum <= 16) {
        JsonDocument data;
        data["relay"] = relayNum;
        data["state"] = _relayController.getState(relayNum) ? "on" : "off";
        sendSuccess(response, data);
    } else {
        sendError(response, "Invalid relay number (1-16)");
    }
}

void CommandHandler::handleAllRelaysOn(JsonDocument& response) {
    _relayController.allOn();
    JsonDocument data;
    sendSuccess(response, data, "All relays turned ON");
    Serial.println("All relays: ON");
}

void CommandHandler::handleAllRelaysOff(JsonDocument& response) {
    _relayController.allOff();
    JsonDocument data;
    sendSuccess(response, data, "All relays turned OFF");
    Serial.println("All relays: OFF");
}

void CommandHandler::handleAllRelayStatus(JsonDocument& response) {
    JsonDocument data;
    JsonArray relays = data["relays"].to<JsonArray>();
    
    for (int i = 1; i <= _relayController.getCount(); i++) {
        relays.add(_relayController.getState(i) ? 1 : 0);
    }
    
    sendSuccess(response, data, "All relay states");
}

// ============================================================================
// SENSOR COMMAND HANDLERS
// ============================================================================

void CommandHandler::handleSensorStatus(JsonDocument& doc, JsonDocument& response) {
    int sensorNum = doc["sensor"] | 0;
    
    float resistance = _sensorController.readResistance(sensorNum);
    int level = _sensorController.readLevel(sensorNum);
    
    if (resistance >= 0 && level >= 0) {
        JsonDocument data;
        data["sensor"] = sensorNum;
        data["resistance"] = round(resistance * 10) / 10.0;
        data["level"] = level;
        sendSuccess(response, data);
    } else {
        sendError(response, "Invalid sensor number (1-3)");
    }
}

void CommandHandler::handleAllSensorStatus(JsonDocument& response) {
    JsonDocument data;
    JsonArray sensors = data["sensors"].to<JsonArray>();
    
    for (int i = 1; i <= _sensorController.getCount(); i++) {
        JsonObject sensor = sensors.add<JsonObject>();
        float resistance = _sensorController.readResistance(i);
        int level = _sensorController.readLevel(i);
        
        sensor["id"] = i;
        sensor["resistance"] = round(resistance * 10) / 10.0;
        sensor["level"] = level;
    }
    
    sendSuccess(response, data, "All sensor readings");
}

// ============================================================================
// COMBINED STATUS HANDLER
// ============================================================================

void CommandHandler::handleAllStatus(JsonDocument& response) {
    JsonDocument data;
    
    // Relays
    JsonArray relays = data["relays"].to<JsonArray>();
    for (int i = 1; i <= _relayController.getCount(); i++) {
        relays.add(_relayController.getState(i) ? 1 : 0);
    }
    
    // Sensors
    JsonArray sensors = data["sensors"].to<JsonArray>();
    for (int i = 1; i <= _sensorController.getCount(); i++) {
        JsonObject sensor = sensors.add<JsonObject>();
        float resistance = _sensorController.readResistance(i);
        int level = _sensorController.readLevel(i);
        
        sensor["id"] = i;
        sensor["resistance"] = round(resistance * 10) / 10.0;
        sensor["level"] = level;
    }
    
    sendSuccess(response, data, "Complete system status");
}

// ============================================================================
// HELPER METHODS
// ============================================================================

void CommandHandler::sendSuccess(JsonDocument& response, JsonDocument& data, const char* message) {
    response["status"] = "ok";
    response["data"] = data;
    if (strlen(message) > 0) {
        response["message"] = message;
    }
}

void CommandHandler::sendError(JsonDocument& response, const char* message) {
    response["status"] = "error";
    response["data"].to<JsonObject>();  // Empty data object
    response["message"] = message;
}
