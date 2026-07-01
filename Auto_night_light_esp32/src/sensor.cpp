#include <Arduino.h>
#include "sensor.h"
#include "state.h"
#include "config.h"

static unsigned long lastRead = 0;

void sensorInit() {
  pinMode(LDR_PIN, INPUT);
  ledcSetup(PWM_CHANNEL, PWM_FREQ, PWM_RES);   // configure the PWM channel
  ledcAttachPin(LED_PIN, PWM_CHANNEL);          // attach the pin to that channel
}

void sensorUpdate() {
  if (millis() - lastRead < READ_INTERVAL_MS) return;
  lastRead = millis();

  lightLevel = analogRead(LDR_PIN);

  if (autoMode) {
    if (lightLevel < lightThreshold) {
      brightness = constrain(map(lightLevel, 0, lightThreshold, 255, 0), 0, 255);
      ledOn = true;
    } else {
      brightness = 0;
      ledOn = false;
    }
    ledcWrite(PWM_CHANNEL, brightness);   // old API writes to the CHANNEL, not the pin
  }
}