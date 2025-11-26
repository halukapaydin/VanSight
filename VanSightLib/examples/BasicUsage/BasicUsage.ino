/*
 * VanSightLib Basic Usage Example
 * 
 * This example demonstrates how to use VanSightLib for parsing commands
 * and building responses.
 */

#include <VanSightLib.h>

using namespace VanSight;

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("\n=== VanSightLib Example ===\n");
    
    // Example 1: Parse a relay toggle command
    const char* jsonCmd = "{\"cmd\":\"relay_toggle\",\"relay\":5}";
    
    Command cmd;
    if (CommandParser::parse((const uint8_t*)jsonCmd, strlen(jsonCmd), cmd)) {
        Serial.println("✓ Command parsed successfully!");
        Serial.printf("  Type: %s\n", commandTypeToString(cmd.type));
        Serial.printf("  Relay: %d\n", cmd.params.relay.relayNum);
    } else {
        Serial.println("✗ Failed to parse command");
    }
    
    Serial.println();
    
    // Example 2: Build a response
    Response response;
    response.status = STATUS_OK;
    response.data.relay.relayNum = 5;
    response.data.relay.state = true;
    
    char buffer[MAX_PAYLOAD_SIZE];
    size_t len = ResponseBuilder::build(response, buffer, sizeof(buffer));
    
    Serial.println("✓ Response built:");
    Serial.printf("  JSON: %s\n", buffer);
    Serial.printf("  Length: %d bytes\n", len);
    
    Serial.println("\n=== Example Complete ===");
}

void loop() {
    // Nothing to do
    delay(1000);
}
