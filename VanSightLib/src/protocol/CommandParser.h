#ifndef COMMAND_PARSER_H
#define COMMAND_PARSER_H

#include <ArduinoJson.h>
#include "VanSightProtocol.h"

namespace VanSight {

/**
 * @brief Parses JSON commands into Command structures
 */
class CommandParser {
public:
    /**
     * @brief Parse JSON data into Command structure
     * @param data Raw JSON data
     * @param len Length of data
     * @param cmd Output command structure
     * @return true if parsing successful
     */
    static bool parse(const uint8_t* data, size_t len, Command& cmd);
    
    /**
     * @brief Parse JSON document into Command structure
     * @param doc JSON document
     * @param cmd Output command structure
     * @return true if parsing successful
     */
    static bool parse(JsonDocument& doc, Command& cmd);

private:
    static bool parseRelayToggle(JsonDocument& doc, Command& cmd);
    static bool parseAllRelaysOff(JsonDocument& doc, Command& cmd);
    static bool parseAllStatus(JsonDocument& doc, Command& cmd);
    static bool parseSensorRead(JsonDocument& doc, Command& cmd);
};

} // namespace VanSight

#endif // COMMAND_PARSER_H
