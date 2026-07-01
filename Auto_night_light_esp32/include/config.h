#pragma once

// ---------- WIFI ----------
constexpr const char* WIFI_SSID     = "Solomon";
constexpr const char* WIFI_PASSWORD = "solomon123";

// ---------- PINS ----------
constexpr int LDR_PIN = 34;
constexpr int LED_PIN = 25;

// ---------- PWM ----------
constexpr int PWM_CHANNEL = 0;   // old ledc API addresses channels, not pins
constexpr int PWM_FREQ = 5000;
constexpr int PWM_RES  = 8;      // 8-bit -> 0-255 duty range

// ---------- TIMING ----------
constexpr unsigned long READ_INTERVAL_MS = 200;

// ---------- DEFAULTS ----------
constexpr int DEFAULT_THRESHOLD = 2000;   // 0-4095, ESP32 12-bit ADC