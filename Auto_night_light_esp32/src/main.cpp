#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "Solomon";
const char* password = "solomon123";

void connectWiFi()
{
    WiFi.begin(ssid, password);

    while(WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println();
    Serial.println("WiFi connected! IP address: ");
    Serial.println(WiFi.localIP());
}

// put function declarations here:
int myFunction(int, int);

void setup() {
  // put your setup code here, to run once:
  int result = myFunction(2, 3);
}

void loop() {
  // put your main code here, to run repeatedly:
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}