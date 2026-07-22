#include <LittleFS.h>
#include "routes.h"
#include "actions.h"

void registerRoutes(AsyncWebServer &server) {

  // IMPORTANT: register all /api/* routes BEFORE the static file handler.
  // AsyncWebServer hands each request to the first handler whose canHandle()
  // matches. serveStatic("/") matches almost any path, so if it were
  // registered first it would swallow /api/* requests before they ever
  // reached these handlers below.

  server.on("/api/status", HTTP_GET, [](AsyncWebServerRequest *req){
    req->send(200, "application/json", actionGetStatus());
  });

  server.on("/api/led/on", HTTP_GET, [](AsyncWebServerRequest *req){
    req->send(200, "application/json", actionLedOn());
  });

  server.on("/api/led/off", HTTP_GET, [](AsyncWebServerRequest *req){
    req->send(200, "application/json", actionLedOff());
  });

  server.on("/api/threshold", HTTP_GET, [](AsyncWebServerRequest *req){
    req->send(200, "application/json", actionGetThreshold());
  });

  server.on("/api/threshold", HTTP_POST, [](AsyncWebServerRequest *req){
    if (req->hasParam("value", true)) {
      int value = req->getParam("value", true)->value().toInt();
      req->send(200, "application/json", actionSetThreshold(value));
    } else {
      req->send(400, "application/json", "{\"error\":\"missing value\"}");
    }
  });

  server.on("/api/mode/auto", HTTP_GET, [](AsyncWebServerRequest *req){
    req->send(200, "application/json", actionSetMode("auto"));
  });

  server.on("/api/mode/manual", HTTP_GET, [](AsyncWebServerRequest *req){
    req->send(200, "application/json", actionSetMode("manual"));
  });

  server.on("/api/brightness", HTTP_GET, [](AsyncWebServerRequest *req){
    req->send(200, "application/json", actionGetBrightness());
  });

  server.on("/api/brightness", HTTP_POST, [](AsyncWebServerRequest *req){
    if (req->hasParam("value", true)) {
      int value = req->getParam("value", true)->value().toInt();
      req->send(200, "application/json", actionSetBrightness(value));
    } else {
      req->send(400, "application/json", "{\"error\":\"missing value\"}");
    }
  });

  // Static file handler goes LAST - it's the fallback for everything
  // that isn't an /api/* route (index.html, style.css, script.js, etc).
  server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html");
}
