#include <Arduino.h>
#include "protocol.h"

void testCommandSerialization() {
    Serial.println("=== Command Serialize / Deserialize Test ===");

    Command cmd;
    cmd.type = CommandType::RELAY_TOGGLE;
    cmd.parameter.item_number = 3;

    // Serialize
    String json = JsonCodec::serializeCommand(cmd);
    Serial.print("Serialized Command: ");
    Serial.println(json);

    // Deserialize
    Command parsed;
    bool ok = JsonCodec::deserializeCommand(json, parsed);

    Serial.print("Deserialize OK: ");
    Serial.println(ok ? "YES" : "NO");

    Serial.print("Command Type: ");
    Serial.println(static_cast<int>(parsed.type));

    Serial.print("Item Number: ");
    Serial.println(parsed.parameter.item_number);

    Serial.println();
}

void testResponseSerialization() {
    Serial.println("=== Response Serialize / Deserialize Test ===");

    Response resp{};

    // Fill test data
    for (int i = 0; i < RELAY_COUNT; i++) {
        resp.relays[i] = (i % 2 == 0); // 1,0,1,0...
    }

    for (int i = 0; i < LEVEL_COUNT; i++) {
        resp.levels[i] = (i + 1) * 25;
    }

    // Serialize
    String json = JsonCodec::serializeResponse(resp);
    Serial.print("Serialized Response: ");
    Serial.println(json);

    // Deserialize
    Response parsed{};
    bool ok = JsonCodec::deserializeResponse(json, parsed);

    Serial.print("Deserialize OK: ");
    Serial.println(ok ? "YES" : "NO");

    // Validate relays
    Serial.print("Relays: ");
    for (int i = 0; i < RELAY_COUNT; i++) {
        Serial.print(parsed.relays[i] ? "1" : "0");
        Serial.print(" ");
    }
    Serial.println();

    // Validate levels
    Serial.print("Levels: ");
    for (int i = 0; i < LEVEL_COUNT; i++) {
        Serial.print(parsed.levels[i]);
        Serial.print(" ");
    }
    Serial.println("\n");
}

void setup() {
    Serial.begin(115200);
    delay(1000);

    Serial.println("\n=== JsonCodec Test Started ===\n");

    testCommandSerialization();
    testResponseSerialization();

    Serial.println("=== JsonCodec Test Finished ===");
}

void loop() {
    // nothing
}
