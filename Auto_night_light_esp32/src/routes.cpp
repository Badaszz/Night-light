#include <LittleFS.h>
#include "routes.h"
#include "state.h"
#include "config.h"

static String statusJson() {
  String json = "{\"mode\":\"" + String(autoMode ? "auto" : "manual") +
    "\",\"led\":" + String(ledOn ? "true" : "false") +
    ",\"brightness\":" + String(brightness) +
    ",\"threshold\":" + String(lightThreshold) +
    ",\"lightLevel\":" + String(lightLevel) + "}";
  return json;
}

void registerRoutes(AsyncWebServer &server) {

  // IMPORTANT: register all /api/* routes BEFORE the static file handler.
  // AsyncWebServer hands each request to the first handler whose canHandle()
  // matches. serveStatic("/") matches almost any path, so if it were
  // registered first it would swallow /api/* requests before they ever
  // reached these handlers below.

  server.on("/api/status", HTTP_GET, [](AsyncWebServerRequest *req){
    req->send(200, "application/json", statusJson());
  });

  server.on("/api/led/on", HTTP_GET, [](AsyncWebServerRequest *req){
    autoMode = false;
    ledOn = true;
    brightness = 255;
    ledcWrite(PWM_CHANNEL, brightness);
    req->send(200, "application/json", statusJson());
  });

  server.on("/api/led/off", HTTP_GET, [](AsyncWebServerRequest *req){
    autoMode = false;
    ledOn = false;
    brightness = 0;
    ledcWrite(PWM_CHANNEL, brightness);
    req->send(200, "application/json", statusJson());
  });

  server.on("/api/threshold", HTTP_GET, [](AsyncWebServerRequest *req){
    req->send(200, "application/json", "{\"threshold\":" + String(lightThreshold) + "}");
  });

  server.on("/api/threshold", HTTP_POST, [](AsyncWebServerRequest *req){
    if (req->hasParam("value", true)) {
      lightThreshold = constrain(req->getParam("value", true)->value().toInt(), 0, 4095);
    }
    req->send(200, "application/json", "{\"threshold\":" + String(lightThreshold) + "}");
  });

  server.on("/api/mode/auto", HTTP_GET, [](AsyncWebServerRequest *req){
    autoMode = true;
    req->send(200, "application/json", statusJson());
  });

  server.on("/api/mode/manual", HTTP_GET, [](AsyncWebServerRequest *req){
    autoMode = false;
    req->send(200, "application/json", statusJson());
  });

  server.on("/api/brightness", HTTP_GET, [](AsyncWebServerRequest *req){
    req->send(200, "application/json", "{\"brightness\":" + String(brightness) + "}");
  });

  server.on("/api/brightness", HTTP_POST, [](AsyncWebServerRequest *req){
    if (req->hasParam("value", true)) {
      autoMode = false;
      brightness = constrain(req->getParam("value", true)->value().toInt(), 0, 255);
      ledOn = brightness > 0;
      ledcWrite(PWM_CHANNEL, brightness);
    }
    req->send(200, "application/json", statusJson());
  });

  // Static file handler goes LAST - it's the fallback for everything
  // that isn't an /api/* route (index.html, style.css, script.js, etc).
  server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html");
}