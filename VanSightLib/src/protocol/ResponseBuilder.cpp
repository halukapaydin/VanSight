#include "ResponseBuilder.h"
#include "../config/VanSightConfig.h"

namespace VanSight {

size_t ResponseBuilder::build(const Response& response, char* buffer, size_t bufferSize) {
    JsonDocument doc;
    build(response, doc);
    return serializeJson(doc, buffer, bufferSize);
}

void ResponseBuilder::build(const Response& response, JsonDocument& doc) {
    // Set status
    doc["status"] = (response.status == STATUS_OK) ? "ok" : "error";
    
    // Build response based on status
    if (response.status != STATUS_OK) {
        buildErrorResponse(response, doc);
        return;
    }
    
    // Create data object
    JsonObject data = doc.createNestedObject("data");
    
    // Note: We need to know which type of response this is
    // For now, we'll check if relay data looks valid
    // In a real implementation, you might want to add a type field to Response
    
    // This is a simplified version - you may want to add a response type field
    doc["data"] = data;
}

void ResponseBuilder::buildRelayResponse(const Response& response, JsonDocument& doc) {
    JsonObject data = doc["data"];
    data["relay"] = response.data.relay.relayNum;
    data["state"] = response.data.relay.state;
}

void ResponseBuilder::buildSensorResponse(const Response& response, JsonDocument& doc) {
    JsonObject data = doc["data"];
    data["sensor"] = response.data.sensor.sensorNum;
    data["level"] = response.data.sensor.level;
}

void ResponseBuilder::buildAllStatusResponse(const Response& response, JsonDocument& doc) {
    JsonObject data = doc["data"];
    
    // Build relays array
    JsonArray relays = data.createNestedArray("relays");
    for (int i = 0; i < MAX_RELAYS; i++) {
        relays.add(response.data.allStatus.relayStates[i]);
    }
    
    // Build sensors array
    JsonArray sensors = data.createNestedArray("sensors");
    for (int i = 0; i < MAX_SENSORS; i++) {
        JsonObject sensor = sensors.createNestedObject();
        sensor["id"] = i + 1;
        sensor["level"] = response.data.allStatus.sensorLevels[i];
    }
}

void ResponseBuilder::buildErrorResponse(const Response& response, JsonDocument& doc) {
    const char* errorMsg;
    switch (response.status) {
        case STATUS_INVALID_COMMAND:
            errorMsg = "Invalid command";
            break;
        case STATUS_INVALID_PARAMETER:
            errorMsg = "Invalid parameter";
            break;
        case STATUS_TIMEOUT:
            errorMsg = "Timeout";
            break;
        default:
            errorMsg = "Unknown error";
            break;
    }
    doc["error"] = errorMsg;
}

} // namespace VanSight
