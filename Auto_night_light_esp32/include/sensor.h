#pragma once

// Sets up the LDR pin and the LED's PWM channel. Call once in setup().
void sensorInit();

// Reads the LDR (rate-limited internally) and, if autoMode is on,
// updates brightness/ledOn and drives the LED. Call every loop().
void sensorUpdate();
