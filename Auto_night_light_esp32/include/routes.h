#pragma once
#include <ESPAsyncWebServer.h>

// Registers every API endpoint and the static dashboard file server
// onto the given server instance. Call once in setup(), after WiFi connects.
void registerRoutes(AsyncWebServer &server);
