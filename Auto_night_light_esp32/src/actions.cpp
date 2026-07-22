#include "actions.h"
#include "state.h"
#include "config.h"

static String statusJson() {
  return "{\"mode\":\"" + String(autoMode ? "auto" : "manual") +
    "\",\"led\":" + String(ledOn ? "true" : "false") +
    ",\"brightness\":" + String(brightness) +
    ",\"threshold\":" + String(lightThreshold) +
    ",\"lightLevel\":" + String(lightLevel) + "}";
}

String actionGetStatus() {
  return statusJson();
}

String actionLedOn() {
  autoMode = false;
  ledOn = true;
  brightness = 255;
  ledcWrite(PWM_CHANNEL, brightness);
  return statusJson();
}

String actionLedOff() {
  autoMode = false;
  ledOn = false;
  brightness = 0;
  ledcWrite(PWM_CHANNEL, brightness);
  return statusJson();
}

String actionGetThreshold() {
  return "{\"threshold\":" + String(lightThreshold) + "}";
}

String actionSetThreshold(int value) {
  lightThreshold = constrain(value, 0, 4095);
  return "{\"threshold\":" + String(lightThreshold) + "}";
}

String actionSetMode(const String &mode) {
  if (mode == "auto") {
    autoMode = true;
  } else if (mode == "manual") {
    autoMode = false;
  } else {
    return "{\"error\":\"mode must be 'auto' or 'manual'\"}";
  }
  return statusJson();
}

String actionGetBrightness() {
  return "{\"brightness\":" + String(brightness) + "}";
}

String actionSetBrightness(int value) {
  autoMode = false;
  brightness = constrain(value, 0, 255);
  ledOn = brightness > 0;
  ledcWrite(PWM_CHANNEL, brightness);
  return statusJson();
}
