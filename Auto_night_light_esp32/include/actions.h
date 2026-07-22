#pragma once
#include <Arduino.h>

// Each function here performs one real action (or read) against the
// night light's state, and returns a JSON string describing the result.
// Both the REST routes (routes.cpp) and the MCP tool dispatcher
// (mcp_server.cpp) call these same functions - so the two interfaces
// can never drift apart or behave differently for the same command.

String actionGetStatus();
String actionLedOn();
String actionLedOff();
String actionGetThreshold();
String actionSetThreshold(int value);
String actionSetMode(const String &mode);
String actionGetBrightness();
String actionSetBrightness(int value);
