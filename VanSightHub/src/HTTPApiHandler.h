#ifndef HTTP_API_HANDLER_H
#define HTTP_API_HANDLER_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include "CommandHandler.h"

/**
 * @brief HTTPApiHandler class for managing HTTP API endpoints
 * 
 * This class provides HTTP API endpoints and integrates with CommandHandler
 */
class HTTPApiHandler {
public:
    /**
     * @brief Construct a new HTTP API Handler object
     * 
     * @param server Reference to AsyncWebServer
     * @param cmdHandler Reference to CommandHandler
     */
    HTTPApiHandler(AsyncWebServer& server, CommandHandler& cmdHandler);
    
    /**
     * @brief Initialize all HTTP endpoints
     */
    void begin();

private:
    AsyncWebServer& _server;
    CommandHandler& _commandHandler;
    
    // Helper methods
    void setupRoutes();
    void handleAPIRequest(AsyncWebServerRequest *request, const char* endpoint, const String& cmd);
    void logRequest(const char* method, const char* endpoint, const String& params = "");
    void logResponse(JsonDocument& response);
};

#endif // HTTP_API_HANDLER_H
