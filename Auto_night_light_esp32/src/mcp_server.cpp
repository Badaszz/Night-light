#include <ArduinoJson.h>
#include <AsyncJson.h>
#include "mcp_server.h"
#include "actions.h"

// ---------------------------------------------------------------------
// TOOL SCHEMAS
// This is the MCP equivalent of what @mcp.tool() + docstrings auto-
// generated in the Python version. Each tool needs a name, a
// human/LLM-readable description, and a JSON Schema describing its
// arguments so the calling LLM knows what to pass in.
// ---------------------------------------------------------------------
static void buildToolsList(JsonArray tools) {

  auto noArgsSchema = [](JsonObject t) {
    JsonObject schema = t["inputSchema"].to<JsonObject>();
    schema["type"] = "object";
    schema["properties"].to<JsonObject>();   // no properties needed
  };

  JsonObject t;

  t = tools.add<JsonObject>();
  t["name"] = "esp32_get_status";
  t["description"] = "Get the full current status: mode (auto/manual), LED on/off, brightness (0-255), threshold (0-4095), and raw light sensor level.";
  noArgsSchema(t);

  t = tools.add<JsonObject>();
  t["name"] = "esp32_led_on";
  t["description"] = "Turn the LED on at full brightness. Switches the device to manual mode.";
  noArgsSchema(t);

  t = tools.add<JsonObject>();
  t["name"] = "esp32_led_off";
  t["description"] = "Turn the LED off. Switches the device to manual mode.";
  noArgsSchema(t);

  t = tools.add<JsonObject>();
  t["name"] = "esp32_get_threshold";
  t["description"] = "Get the current auto-mode light threshold (0-4095) - the raw LDR reading below which the LED turns on automatically.";
  noArgsSchema(t);

  t = tools.add<JsonObject>();
  t["name"] = "esp32_set_threshold";
  t["description"] = "Set the auto-mode light threshold. Higher values turn the LED on earlier (at brighter ambient light).";
  {
    JsonObject schema = t["inputSchema"].to<JsonObject>();
    schema["type"] = "object";
    JsonObject props = schema["properties"].to<JsonObject>();
    JsonObject value = props["value"].to<JsonObject>();
    value["type"] = "integer";
    value["description"] = "New threshold, 0-4095";
    JsonArray req = schema["required"].to<JsonArray>();
    req.add("value");
  }

  t = tools.add<JsonObject>();
  t["name"] = "esp32_set_mode";
  t["description"] = "Switch between automatic (sensor-driven) and manual light control.";
  {
    JsonObject schema = t["inputSchema"].to<JsonObject>();
    schema["type"] = "object";
    JsonObject props = schema["properties"].to<JsonObject>();
    JsonObject mode = props["mode"].to<JsonObject>();
    mode["type"] = "string";
    JsonArray enumArr = mode["enum"].to<JsonArray>();
    enumArr.add("auto");
    enumArr.add("manual");
    JsonArray req = schema["required"].to<JsonArray>();
    req.add("mode");
  }

  t = tools.add<JsonObject>();
  t["name"] = "esp32_get_brightness";
  t["description"] = "Get the current LED brightness (0-255).";
  noArgsSchema(t);

  t = tools.add<JsonObject>();
  t["name"] = "esp32_set_brightness";
  t["description"] = "Set the LED brightness directly (0-255). Switches the device to manual mode.";
  {
    JsonObject schema = t["inputSchema"].to<JsonObject>();
    schema["type"] = "object";
    JsonObject props = schema["properties"].to<JsonObject>();
    JsonObject value = props["value"].to<JsonObject>();
    value["type"] = "integer";
    value["description"] = "Brightness, 0-255";
    JsonArray req = schema["required"].to<JsonArray>();
    req.add("value");
  }
}

// ---------------------------------------------------------------------
// DISPATCH - map a tool name + arguments to an actual action call
// ---------------------------------------------------------------------
static String callTool(const String &name, JsonObject args) {
  if (name == "esp32_get_status")     return actionGetStatus();
  if (name == "esp32_led_on")         return actionLedOn();
  if (name == "esp32_led_off")        return actionLedOff();
  if (name == "esp32_get_threshold")  return actionGetThreshold();
  if (name == "esp32_set_threshold")  return actionSetThreshold(args["value"] | 0);
  if (name == "esp32_set_mode")       return actionSetMode(String((const char*)(args["mode"] | "")));
  if (name == "esp32_get_brightness") return actionGetBrightness();
  if (name == "esp32_set_brightness") return actionSetBrightness(args["value"] | 0);
  return "{\"error\":\"unknown tool: " + name + "\"}";
}

// ---------------------------------------------------------------------
// MAIN REQUEST HANDLER - every /mcp POST lands here
// ---------------------------------------------------------------------
static void handleMcpRequest(AsyncWebServerRequest *request, JsonVariant &json) {
  JsonObject body = json.as<JsonObject>();

  String method = body["method"] | "";
  JsonVariant id = body["id"];

  JsonDocument responseDoc;
  responseDoc["jsonrpc"] = "2.0";
  responseDoc["id"] = id;

  if (method == "initialize") {
    JsonObject result = responseDoc["result"].to<JsonObject>();
    result["protocolVersion"] = "2024-11-05";
    JsonObject capabilities = result["capabilities"].to<JsonObject>();
    capabilities["tools"].to<JsonObject>();   // announce tool support
    JsonObject serverInfo = result["serverInfo"].to<JsonObject>();
    serverInfo["name"] = "esp32-night-light";
    serverInfo["version"] = "1.0.0";
  }
  else if (method == "tools/list") {
    JsonObject result = responseDoc["result"].to<JsonObject>();
    JsonArray tools = result["tools"].to<JsonArray>();
    buildToolsList(tools);
  }
  else if (method == "tools/call") {
    JsonObject params = body["params"];
    String toolName = params["name"] | "";
    JsonObject args = params["arguments"];

    String toolResult = callTool(toolName, args);

    JsonObject result = responseDoc["result"].to<JsonObject>();
    JsonArray content = result["content"].to<JsonArray>();
    JsonObject item = content.add<JsonObject>();
    item["type"] = "text";
    item["text"] = toolResult;
  }
  else {
    JsonObject error = responseDoc["error"].to<JsonObject>();
    error["code"] = -32601;
    error["message"] = "Method not found";
  }

  String output;
  serializeJson(responseDoc, output);
  request->send(200, "application/json", output);
}

void registerMcpRoute(AsyncWebServer &server) {
  static AsyncCallbackJsonWebHandler *handler =
      new AsyncCallbackJsonWebHandler("/mcp", handleMcpRequest);
  server.addHandler(handler);
}
