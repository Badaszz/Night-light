# Auto Night Light ESP32

This project is a modular ESP32-based night-light controller with an automatic light-sensing mode, a manual override mode, and a small web dashboard that runs directly on the device. The firmware is designed to be easy to read and extend, with separate modules for configuration, sensor handling, shared state, and HTTP routes.

The ESP32 connects to Wi-Fi, serves a local website from its filesystem, and exposes a small set of HTTP endpoints that can be called directly from the browser, a script, or the MCP-based assistant in the parent project.

## What the project does

The device behaves like a smart night light:

- It reads a light-dependent resistor (LDR) from the environment.
- In auto mode, it decides whether the LED should turn on based on the ambient light level and a configurable threshold.
- In manual mode, the LED can be controlled directly.
- It hosts a local web dashboard with HTML, CSS, and JavaScript so you can monitor and control it from a browser.
- It exposes simple REST-style endpoints for status, mode switching, brightness control, and threshold adjustment.

## Project structure

The project is intentionally split into small modules so it is easy to understand and maintain.

### [src](src)
This folder contains the main application logic.

- [src/main.cpp](src/main.cpp)  
  Entry point for the firmware. Initializes Wi-Fi, sensor handling, the web server, and the route registration.

- [src/sensor.cpp](src/sensor.cpp)  
  Handles the LDR input and PWM LED output. This is where the light reading and automatic brightness logic live.

- [src/state.cpp](src/state.cpp)  
  Stores the shared runtime state such as auto/manual mode, LED state, brightness, threshold, and last light level.

- [src/routes.cpp](src/routes.cpp)  
  Registers all the HTTP routes and defines the API behavior for the web dashboard and external callers.

### [include](include)
This folder contains the header files used by the firmware.

- [include/config.h](include/config.h)  
  Hardware pin assignments, Wi-Fi credentials, PWM settings, and default values.

- [include/sensor.h](include/sensor.h)  
  Declares the sensor initialization and update functions.

- [include/state.h](include/state.h)  
  Declares the shared state variables used across modules.

- [include/routes.h](include/routes.h)  
  Declares the route registration function.

### [data](data)
This folder contains the files that are uploaded to the ESP32 filesystem and served as the web dashboard.

- [data/index.html](data/index.html)  
  Main page layout for the dashboard.

- [data/style.css](data/style.css)  
  Styling for the dashboard UI.

- [data/script.js](data/script.js)  
  JavaScript that fetches status and sends control requests to the ESP32 API.

### [test](test)
Placeholder folder for future tests.

### [lib](lib)
Placeholder folder for custom libraries if I add more modules later.

## Hardware overview

The firmware expects the following hardware connections:

- An LDR (light-dependent resistor) connected as a voltage divider to the ESP32 ADC input.
- An LED connected to a PWM-capable GPIO pin.
- A common ground between the ESP32 and the circuit.

### Suggested circuit wiring

#### 1. LDR sensor
Use the LDR as part of a simple voltage divider:

- One leg of the LDR to 3.3V
- The other leg of the LDR to GPIO34 (ADC input)
- A 10k resistor from GPIO34 to GND

This creates a variable voltage that the ESP32 can read through the ADC.

#### 2. LED output
Connect the LED to the output pin:

- LED anode to GPIO25 through a 220Ω resistor
- LED cathode to GND

If you use a brighter LED or a larger load, consider using a transistor or MOSFET driver for safety.

#### 3. Power and ground
- Connect the ESP32 3.3V pin to the 3.3V rail used by the sensor and LED circuit.
- Connect all grounds together.

## Configuration

Before flashing the firmware, update the Wi-Fi and pin settings in [include/config.h](include/config.h).

Important values to review:

- `WIFI_SSID`
- `WIFI_PASSWORD`
- `LDR_PIN`
- `LED_PIN`
- `DEFAULT_THRESHOLD`

The default threshold is `2000` and the ADC range is `0-4095`, which works well as a starting point.

## How to run it with PlatformIO

This project is built for PlatformIO.

### 1. Install PlatformIO
If you are using VS Code, install the PlatformIO IDE extension.

You can also use PlatformIO Core from the command line if you prefer.

### 2. Open the project folder
Open the [Auto_night_light_esp32](.) folder in VS Code as a PlatformIO project.

### 3. Build and upload the firmware
From the PlatformIO toolbar or command palette:

- Build the project
- Upload the firmware to the ESP32

### 4. Upload the web files
Because the dashboard is served from the ESP32 filesystem, you also need to upload the files in [data](data):

- Use the PlatformIO task for “Upload Filesystem Image”

If you are using the CLI, this is typically done with:

```bash
pio run --target uploadfs
```

### 5. Monitor the serial output
Open the serial monitor at 115200 baud to see the ESP32 connect to Wi-Fi and print its local IP address.

### 6. Open the web dashboard
Once the ESP32 is connected to Wi-Fi, open a browser and visit:

```text
http://<ESP32_IP>
```

You should see the night-light dashboard.

## Web dashboard

The device serves a small website from its internal filesystem. The dashboard lets you:

- View the current LED state
- See the current brightness value
- View the latest raw light sensor reading
- Adjust the auto-mode threshold
- Switch between auto and manual modes
- Turn the LED on or off manually

The UI is built with plain HTML, CSS, and JavaScript and is served from [data/index.html](data/index.html), [data/style.css](data/style.css), and [data/script.js](data/script.js).

## API endpoints

The ESP32 also exposes several endpoints that can be called directly.

### Status
- `GET /api/status`  
  Returns the current mode, LED state, brightness, threshold, and light level.

### Manual LED control
- `GET /api/led/on`  
  Turns the LED on and switches to manual mode.
- `GET /api/led/off`  
  Turns the LED off and switches to manual mode.

### Auto threshold
- `GET /api/threshold`  
  Returns the current threshold.
- `POST /api/threshold` with form field `value`  
  Sets the threshold.

### Mode control
- `GET /api/mode/auto`  
  Switches to automatic mode.
- `GET /api/mode/manual`  
  Switches to manual mode.

### Brightness control
- `GET /api/brightness`  
  Returns the current brightness.
- `POST /api/brightness` with form field `value`  
  Sets the brightness directly and switches to manual mode.

These endpoints are useful both for the browser-based dashboard and for the MCP-based assistant in the parent folder.

## Why the architecture is modular

This codebase is intentionally structured so each concern has its own place:

- [include/config.h](include/config.h) keeps hardware and network settings centralized.
- [src/sensor.cpp](src/sensor.cpp) isolates sensor and LED behavior.
- [src/state.cpp](src/state.cpp) keeps shared state in one place.
- [src/routes.cpp](src/routes.cpp) keeps HTTP API logic separate from the rest of the firmware.
- [data](data) holds the UI assets independently from the firmware logic.

That makes it easier to read, test, and extend the project later.

## Notes

- The ESP32 must be connected to your Wi-Fi network before the dashboard becomes reachable.
- If the web page does not load, make sure the filesystem image was uploaded successfully.
- The serial monitor is useful for debugging connection issues or unexpected behavior.
