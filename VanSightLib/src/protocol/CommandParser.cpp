#include "CommandParser.h"
#include "../config/VanSightConfig.h"

namespace VanSight {

bool CommandParser::parse(const uint8_t* data, size_t len, Command& cmd) {
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, data, len);
    
    if (error) {
        Serial.printf("[CommandParser] JSON parse error: %s\n", error.c_str());
        return false;
    }
    
    return parse(doc, cmd);
}

bool CommandParser::parse(JsonDocument& doc, Command& cmd) {
    const char* cmdStr = doc["cmd"];
    if (!cmdStr) {
        Serial.println("[CommandParser] Missing 'cmd' field");
        return false;
    }
    
    cmd.type = stringToCommandType(cmdStr);
    
    switch (cmd.type) {
        case CMD_RELAY_TOGGLE:
            return parseRelayToggle(doc, cmd);
        case CMD_ALL_RELAYS_OFF:
            return parseAllRelaysOff(doc, cmd);
        case CMD_ALL_STATUS:
            return parseAllStatus(doc, cmd);
        case CMD_SENSOR_READ:
            return parseSensorRead(doc, cmd);
        default:
            Serial.printf("[CommandParser] Unknown command: %s\n", cmdStr);
            return false;
    }
}

bool CommandParser::parseRelayToggle(JsonDocument& doc, Command& cmd) {
    if (!doc.containsKey("relay")) {
        Serial.println("[CommandParser] Missing 'relay' parameter");
        return false;
    }
    
    cmd.params.relay.relayNum = doc["relay"];
    
    if (cmd.params.relay.relayNum < 1 || cmd.params.relay.relayNum > MAX_RELAYS) {
        Serial.printf("[CommandParser] Invalid relay number: %d\n", cmd.params.relay.relayNum);
        return false;
    }
    
    return true;
}

bool CommandParser::parseAllRelaysOff(JsonDocument& doc, Command& cmd) {
    // No parameters needed
    return true;
}

bool CommandParser::parseAllStatus(JsonDocument& doc, Command& cmd) {
    // No parameters needed
    return true;
}

bool CommandParser::parseSensorRead(JsonDocument& doc, Command& cmd) {
    if (!doc.containsKey("sensor")) {
        Serial.println("[CommandParser] Missing 'sensor' parameter");
        return false;
    }
    
    cmd.params.sensor.sensorNum = doc["sensor"];
    
    if (cmd.params.sensor.sensorNum < 1 || cmd.params.sensor.sensorNum > MAX_SENSORS) {
        Serial.printf("[CommandParser] Invalid sensor number: %d\n", cmd.params.sensor.sensorNum);
        return false;
    }
    
    return true;
}

} // namespace VanSight
