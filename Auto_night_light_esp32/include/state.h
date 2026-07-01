#pragma once

// These are DECLARATIONS - "this variable exists somewhere, trust me".
// The actual variables live in state.cpp. Any file that includes this
// header can read/write them.

extern bool autoMode;
extern bool ledOn;
extern int  brightness;      // 0-255
extern int  lightThreshold;  // 0-4095
extern int  lightLevel;      // last raw LDR reading
