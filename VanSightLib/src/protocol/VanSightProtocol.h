#ifndef VANSIGHT_PROTOCOL_H
#define VANSIGHT_PROTOCOL_H

#include <Arduino.h>

namespace VanSight {

// Command Types
enum CommandType {
    CMD_RELAY_TOGGLE = 0,
    CMD_ALL_RELAYS_OFF = 1,
    CMD_ALL_STATUS = 2,
    CMD_SENSOR_READ = 3,
    CMD_UNKNOWN = 255
};

// Response Status
enum ResponseStatus {
    STATUS_OK = 0,
    STATUS_ERROR = 1,
    STATUS_INVALID_COMMAND = 2,
    STATUS_INVALID_PARAMETER = 3,
    STATUS_TIMEOUT = 4
};

// Command Structure
struct Command {
    CommandType type;
    union {
        struct {
            uint8_t relayNum;  // For CMD_RELAY_TOGGLE
        } relay;
        struct {
            uint8_t sensorNum; // For CMD_SENSOR_READ
        } sensor;
    } params;
    
    Command() : type(CMD_UNKNOWN) {}
};

// Response Structure
struct Response {
    ResponseStatus status;
    union {
        struct {
            uint8_t relayNum;
            int state;
        } relay;
        struct {
            uint8_t sensorNum;
            int level;
        } sensor;
        struct {
            int relayStates[16];
            int sensorLevels[3];
        } allStatus;
    } data;
    
    Response() : status(STATUS_OK) {}
};

// Helper function to convert CommandType to string
inline const char* commandTypeToString(CommandType type) {
    switch (type) {
        case CMD_RELAY_TOGGLE: return "relay_toggle";
        case CMD_ALL_RELAYS_OFF: return "all_relays_off";
        case CMD_ALL_STATUS: return "all_status";
        case CMD_SENSOR_READ: return "sensor_read";
        default: return "unknown";
    }
}

// Helper function to convert string to CommandType
inline CommandType stringToCommandType(const char* str) {
    if (strcmp(str, "relay_toggle") == 0) return CMD_RELAY_TOGGLE;
    if (strcmp(str, "all_relays_off") == 0) return CMD_ALL_RELAYS_OFF;
    if (strcmp(str, "all_status") == 0) return CMD_ALL_STATUS;
    if (strcmp(str, "sensor_read") == 0) return CMD_SENSOR_READ;
    return CMD_UNKNOWN;
}

// All Status Data Structure (used by CommandManager and BleCommandManager)
struct AllStatusData {
    int relayStates[16];  // MAX_RELAYS
    int sensorLevels[3];   // MAX_SENSORS
};

} // namespace VanSight

#endif // VANSIGHT_PROTOCOL_H
