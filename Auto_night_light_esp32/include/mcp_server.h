#pragma once
#include <ESPAsyncWebServer.h>

// Registers a single POST /mcp endpoint that speaks MCP's JSON-RPC 2.0
// protocol (initialize, tools/list, tools/call). Call once in setup(),
// alongside registerRoutes().
void registerMcpRoute(AsyncWebServer &server);
