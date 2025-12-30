#include "JsonCodec.h"

/* ===================== SINGLETON ===================== */

JsonCodec& JsonCodec::instance() {
    static JsonCodec inst;
    return inst;
}

/* ===================== ENUM HELPERS ===================== */

const char* JsonCodec::commandTypeToString(CommandType type) {
    switch (type) {
        case CommandType::RELAY_TOGGLE:   return "RELAY_TOGGLE";
        case CommandType::RELAY_ON:       return "RELAY_ON";
        case CommandType::RELAY_OFF:      return "RELAY_OFF";
        case CommandType::LEVEL_STATUS:   return "LEVEL_STATUS";
        case CommandType::ALL_RELAYS_OFF: return "ALL_RELAYS_OFF";
        case CommandType::ALL_STATUS:     return "ALL_STATUS";
        default:                          return "UNKNOWN";
    }
}

CommandType JsonCodec::stringToCommandType(const String& s) {
    if (s == "RELAY_TOGGLE")   return CommandType::RELAY_TOGGLE;
    if (s == "RELAY_ON")       return CommandType::RELAY_ON;
    if (s == "RELAY_OFF")      return CommandType::RELAY_OFF;
    if (s == "LEVEL_STATUS")   return CommandType::LEVEL_STATUS;
    if (s == "ALL_RELAYS_OFF") return CommandType::ALL_RELAYS_OFF;
    if (s == "ALL_STATUS")     return CommandType::ALL_STATUS;
    return CommandType::UNKNOWN;
}

/* ===================== COMMAND ===================== */

String JsonCodec::serializeCommand(const Command& cmd) {
    String json = "{";
    json += "\"type\":\"";
    json += commandTypeToString(cmd.type);
    json += "\"";

    if (cmd.parameter.item_number >= 0) {
        json += ",\"item\":";
        json += cmd.parameter.item_number;
    }

    json += "}";
    return json;
}

Command JsonCodec::deserializeCommand(const String& json) {
    Command cmd;

    String typeStr;
    if (extractString(json, "\"type\"", typeStr))
        cmd.type = stringToCommandType(typeStr);

    extractInt(json, "\"item\"", cmd.parameter.item_number);
    return cmd;
}

/* ===================== RESPONSE ===================== */

String JsonCodec::serializeResponse(const Response& res) {
    String json = "{";

    json += "\"relays\":[";
    for (int i = 0; i < RELAY_COUNT; i++) {
        json += res.relays[i] ? "true" : "false";
        if (i < RELAY_COUNT - 1) json += ",";
    }
    json += "],";

    json += "\"levels\":[";
    for (int i = 0; i < LEVEL_COUNT; i++) {
        json += res.levels[i];
        if (i < LEVEL_COUNT - 1) json += ",";
    }
    json += "]";

    json += "}";
    return json;
}

bool JsonCodec::deserializeResponse(const String& json, Response& res) {
    return parseBoolArray(json, "\"relays\"", res.relays, RELAY_COUNT) &&
           parseIntArray(json, "\"levels\"", res.levels, LEVEL_COUNT);
}

/* ===================== HELPERS ===================== */

bool JsonCodec::extractString(const String& json, const char* key, String& out) {
    int keyPos = json.indexOf(key);
    if (keyPos < 0) return false;

    int start = json.indexOf('"', keyPos + strlen(key));
    int end   = json.indexOf('"', start + 1);
    if (start < 0 || end < 0) return false;

    out = json.substring(start + 1, end);
    return true;
}

bool JsonCodec::extractInt(const String& json, const char* key, int& out) {
    int keyPos = json.indexOf(key);
    if (keyPos < 0) return false;

    int colon = json.indexOf(':', keyPos);
    int end   = json.indexOf(',', colon);
    if (end < 0) end = json.indexOf('}', colon);

    out = json.substring(colon + 1, end).toInt();
    return true;
}

bool JsonCodec::parseBoolArray(const String& json, const char* key, bool* arr, int count) {
    int keyPos = json.indexOf(key);
    if (keyPos < 0) return false;

    int start = json.indexOf('[', keyPos);
    int end   = json.indexOf(']', start);
    if (start < 0 || end < 0) return false;

    String data = json.substring(start + 1, end);
    int idx = 0, pos = 0;

    while (idx < count) {
        int comma = data.indexOf(',', pos);
        String token = (comma < 0) ? data.substring(pos)
                                   : data.substring(pos, comma);
        token.trim();
        arr[idx++] = (token == "true");
        if (comma < 0) break;
        pos = comma + 1;
    }
    return idx == count;
}

bool JsonCodec::parseIntArray(const String& json, const char* key, int* arr, int count) {
    int keyPos = json.indexOf(key);
    if (keyPos < 0) return false;

    int start = json.indexOf('[', keyPos);
    int end   = json.indexOf(']', start);
    if (start < 0 || end < 0) return false;

    String data = json.substring(start + 1, end);
    int idx = 0, pos = 0;

    while (idx < count) {
        int comma = data.indexOf(',', pos);
        String token = (comma < 0) ? data.substring(pos)
                                   : data.substring(pos, comma);
        token.trim();
        arr[idx++] = token.toInt();
        if (comma < 0) break;
        pos = comma + 1;
    }
    return idx == count;
}
