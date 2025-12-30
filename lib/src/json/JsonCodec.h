//
// Created by haluk on 12/29/25.
//

#ifndef LIB_JSONCODEC_H
#define LIB_JSONCODEC_H

#pragma once
#include <Arduino.h>

#include "../protocol/protocol.h"

class JsonCodec {
public:
    static JsonCodec& instance();

    /* ---- Command ---- */
    String serializeCommand(const Command& cmd);
    Command deserializeCommand(const String& json);

    /* ---- Response ---- */
    String serializeResponse(const Response& res);
    bool deserializeResponse(const String& json, Response& res);

private:
    JsonCodec() = default;
    JsonCodec(const JsonCodec&) = delete;
    JsonCodec& operator=(const JsonCodec&) = delete;

    /* ---- helpers ---- */
    const char* commandTypeToString(CommandType type);
    CommandType stringToCommandType(const String& s);

    bool extractString(const String& json, const char* key, String& out);
    bool extractInt(const String& json, const char* key, int& out);

    bool parseBoolArray(const String& json, const char* key, bool* arr, int count);
    bool parseIntArray(const String& json, const char* key, int* arr, int count);
};

#endif //LIB_JSONCODEC_H