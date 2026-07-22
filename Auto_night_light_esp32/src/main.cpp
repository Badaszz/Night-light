#include <Arduino.h>
#include <WiFi.h>
#include <LittleFS.h>
#include <ESPAsyncWebServer.h>

#include "config.h"
#include "state.h"
#include "sensor.h"
#include "routes.h"
#include "mcp_server.h"

AsyncWebServer server(80);

void setup() {
  Serial.begin(115200);

  if (!LittleFS.begin(true)) {   // true = format if mount fails (first boot)
    Serial.println("LittleFS mount failed!");
    return;
  }

  sensorInit();

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected! Dashboard at: http://");
  Serial.println(WiFi.localIP());

  registerRoutes(server);
  registerMcpRoute(server);
  server.begin();
}

void loop() {
  sensorUpdate();
}