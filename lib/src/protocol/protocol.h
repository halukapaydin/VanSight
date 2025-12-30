#ifndef LIB_PROTOCOL_H
#define LIB_PROTOCOL_H

/* ===================== COMMAND ===================== */

enum class CommandType {
    RELAY_TOGGLE,
    RELAY_ON,
    RELAY_OFF,
    LEVEL_STATUS,
    ALL_RELAYS_OFF,
    ALL_STATUS,
    UNKNOWN
};

struct Command {
    CommandType type = CommandType::UNKNOWN;

    struct {
        int item_number = -1;
    } parameter;
};

/* ===================== RESPONSE ===================== */

constexpr int RELAY_COUNT = 16;
constexpr int LEVEL_COUNT = 3;

struct Response {
    bool relays[RELAY_COUNT];
    int  levels[LEVEL_COUNT];
};



#endif //LIB_PROTOCOL_H