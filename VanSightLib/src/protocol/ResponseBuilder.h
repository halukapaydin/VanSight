#ifndef RESPONSE_BUILDER_H
#define RESPONSE_BUILDER_H

#include <ArduinoJson.h>
#include "VanSightProtocol.h"

namespace VanSight {

/**
 * @brief Builds JSON responses from Response structures
 */
class ResponseBuilder {
public:
    /**
     * @brief Build JSON response from Response structure
     * @param response Response structure
     * @param buffer Output buffer
     * @param bufferSize Size of output buffer
     * @return Number of bytes written
     */
    static size_t build(const Response& response, char* buffer, size_t bufferSize);
    
    /**
     * @brief Build JSON document from Response structure
     * @param response Response structure
     * @param doc Output JSON document
     */
    static void build(const Response& response, JsonDocument& doc);

private:
    static void buildRelayResponse(const Response& response, JsonDocument& doc);
    static void buildSensorResponse(const Response& response, JsonDocument& doc);
    static void buildAllStatusResponse(const Response& response, JsonDocument& doc);
    static void buildErrorResponse(const Response& response, JsonDocument& doc);
};

} // namespace VanSight

#endif // RESPONSE_BUILDER_H
