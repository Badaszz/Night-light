# MCP Local Tool Server for an Auto Night Light

This project is a small home-automation experiment that combines Internet of Things (IoT) with artificial intelligence. The goal is to create a pseudo-assistant that can receive natural-language instructions and control an ESP32-based auto night light system through an LLM-powered workflow.

The folder contains:
- a local MCP server that exposes tools
- a client that connects that server to an LLM
- a local-first setup that is designed to work with a locally run model instead of a cloud API

The ESP32 night-light firmware and web-based control code live in the parent folder of this project.

## What this project does

The idea is simple:
1. An LLM is given access to a set of tools through MCP.
2. The MCP server exposes those tools over a local interface.
3. The client connects the LLM to the MCP server and lets the model decide when to call a tool.
4. The tools can do things like:
   - understand natural-language requests
   - inspect and control the ESP32-based night light
   - perform simple helper tasks such as calculations or file reads

This makes it possible to talk to the system in natural language. For example, you can ask the model to turn the night light on, check its current brightness, change the auto threshold, or switch between manual and automatic modes.

## Architecture

### MCP server
The server is implemented in `server.py`.

It uses FastMCP to expose tools to the client. The server currently provides a mix of general-purpose tools and ESP32-specific tools.

### MCP client
The client is implemented in `client.py`.

It:
- starts the MCP server locally
- connects to it over stdio
- asks the MCP server for its available tools
- converts those tools into an OpenAI-style function-calling schema
- sends them to an LLM
- executes tool calls when the model decides to use them

### Local LLM setup
The client is written to work with a local OpenAI-compatible API endpoint. In this setup, that endpoint is provided by llama.cpp running locally.

That means the system is intentionally designed to be fully local:
- no cloud LLM API required
- no data leave your machine for the model inference step
- the whole setup can be run on a normal local computer

## Why this uses a local model

This project was built this way because the main goal was to make the experience completely locally runnable while keeping the assistant practical for home automation.

The model used here is Qwen2.5, run through llama.cpp. I chose Qwen2.5 because it is:
- good at tool calling and function-calling behavior
- lightweight enough to run locally
- practical for CPU-based or low-resource setups
- suitable for systems with limited RAM and no GPU

In practice, this project is intended to work on a machine with:
- no GPU required
- around 8 GB RAM is enough for a lightweight setup

If you prefer, the client can also be adapted to work with OpenAI-style models instead of a local model. The current implementation uses the local route because the goal was to keep everything self-hosted and offline-friendly.

## Prerequisites

Before running this project, make sure you have:
- Python installed
- `uv` installed (the code uses `uv run`)
- a local llama.cpp server running
- a Qwen2.5 Instruct model downloaded in a format compatible with llama.cpp
- the ESP32 night-light firmware running and reachable on your network

## Getting the Qwen2.5 model

To use this project with a local LLM, follow these steps:

1. Download the prebuilt llama.cpp binaries from the official llama.cpp releases page.
2. Go to Hugging Face and follow the instructions for downloading a Qwen2.5 Instruct model.
3. Make sure you download a model file that is compatible with llama.cpp (typically a GGUF model for llama.cpp).
4. Start the llama.cpp server with that model file.

A typical local setup would look like this conceptually:

```bash
./llama-server -m /path/to/qwen2.5-instruct.gguf --host 127.0.0.1 --port 8080
```

The client is configured to call the local OpenAI-compatible endpoint at:

```text
http://localhost:8080/v1
```

If your model uses a different name or alias, update the `MODEL_NAME` value in the client accordingly.

## Installing dependencies

From this folder, install the Python dependencies with:

```bash
uv sync
```

## Configuring the ESP32 connection

The MCP server expects the ESP32 IP address to be available in a `.env` file.

Create a `.env` file in this folder with a line like:

```text
ESP32_IP=192.168.1.50
```

Replace the IP address with the actual address of your ESP32 device.

The ESP32 firmware for this project is in the parent folder, under the ESP32 project directory.

## Running the client

Once the LLM backend and the ESP32 device are ready, run the client from this folder:

```bash
uv run client.py
```

The client will prompt you for a message. You can type requests such as:
- "Turn on the night light"
- "What is the current status of the night light?"
- "Set the brightness to 120"
- "Switch to auto mode"
- "Read the current threshold"

The model may decide to call tools automatically when it thinks that is the right action.

## Example usage

Here is an example of the assistant controlling the ESP32 through natural language:

```text
C:\Users\Yusuf Solomon\Documents\night_light\mcp>uv run client.py

Enter a prompt (or 'quit' to exit): switch the esp32 to manual mode
==================================================
[*] Available tools: ['hello_world', 'calculator', 'funny_calculator', 'read_file', 'get_system_info', 'esp32_get_status', 'esp32_led_on', 'esp32_led_off', 'esp32_get_threshold', 'esp32_set_threshold', 'esp32_set_mode', 'esp32_get_brightness', 'esp32_set_brightness']
[*] User: switch the esp32 to manual mode

[*] LLM calling tool: esp32_set_mode({'mode': 'manual'})
[*] Tool result: {"mode":"manual","led":false,"brightness":0,"threshold":2000,"lightLevel":4095}

[Assistant]: The ESP32 night light mode has been switched to manual mode, the LED is off, the current brightness is 0, the auto-mode threshold is 2000 and the raw light sensor reading is 4095. The light will not turn on until the ambient light drops below the threshold.

Enter a prompt (or 'quit' to exit): 

Enter a prompt (or 'quit' to exit): switch on the led
==================================================
[*] Available tools: ['hello_world', 'calculator', 'funny_calculator', 'read_file', 'get_system_info', 'esp32_get_status', 'esp32_led_on', 'esp32_led_off', 'esp32_get_threshold', 'esp32_set_threshold', 'esp32_set_mode', 'esp32_get_brightness', 'esp32_set_brightness']
[*] User: switch on the led

[*] LLM calling tool: esp32_led_on({})
[*] Tool result: {"mode":"manual","led":true,"brightness":255,"threshold":2000,"lightLevel":4095}

[Assistant]: The LED has been turned on. The current brightness is set to full (255) and the mode is set to manual, meaning the LED will stay at this state unless changed manually. The auto-mode threshold is set to 2000 and the raw light sensor reading is 4095, indicating the room is quite dark.

Enter a prompt (or 'quit' to exit): switch it off
==================================================
[*] Available tools: ['hello_world', 'calculator', 'funny_calculator', 'read_file', 'get_system_info', 'esp32_get_status', 'esp32_led_on', 'esp32_led_off', 'esp32_get_threshold', 'esp32_set_threshold', 'esp32_set_mode', 'esp32_get_brightness', 'esp32_set_brightness']
[*] User: switch it off

[*] LLM calling tool: esp32_led_off({})
[*] Tool result: {"mode":"manual","led":false,"brightness":0,"threshold":2000,"lightLevel":4095}

[Assistant]: The night light has been switched off and is now in manual mode. The LED is off, brightness is at 0, the auto-mode threshold is set to 2000, and the current raw light sensor reading is 4095 (indicating bright ambient light).

Enter a prompt (or 'quit' to exit): switch the esp32 to auto mode
==================================================
[*] Available tools: ['hello_world', 'calculator', 'funny_calculator', 'read_file', 'get_system_info', 'esp32_get_status', 'esp32_led_on', 'esp32_led_off', 'esp32_get_threshold', 'esp32_set_threshold', 'esp32_set_mode', 'esp32_get_brightness', 'esp32_set_brightness']
[*] User: switch the esp32 to auto mode

[*] LLM calling tool: esp32_set_mode({'mode': 'auto'})
[*] Tool result: {"mode":"auto","led":false,"brightness":0,"threshold":2000,"lightLevel":4095}

[Assistant]: The ESP32 night light is now in auto mode and is currently off. The auto-mode threshold is set to 2000 and the raw light sensor reading is at 4095, indicating the room is still quite bright.

Enter a prompt (or 'quit' to exit): what is the current brightness
==================================================
[*] Available tools: ['hello_world', 'calculator', 'funny_calculator', 'read_file', 'get_system_info', 'esp32_get_status', 'esp32_led_on', 'esp32_led_off', 'esp32_get_threshold', 'esp32_set_threshold', 'esp32_set_mode', 'esp32_get_brightness', 'esp32_set_brightness']
[*] User: what is the current brightness

[*] LLM calling tool: esp32_get_brightness({})
[*] Tool result: {"brightness":159}

[Assistant]: The current brightness of the ESP32 night light is 159.

Enter a prompt (or 'quit' to exit): quit
```

## Available tools for the LLM

The following tools are currently exposed to the model through MCP:

### General tools
- `hello_world` — returns a simple greeting string
- `calculator(operation, a, b)` — performs basic arithmetic
- `funny_calculator(operation, a, b)` — performs arithmetic with playful output
- `read_file(file_path)` — reads text from a file on disk
- `get_system_info()` — returns basic system information

### ESP32 night-light tools
- `esp32_get_status()` — returns the current ESP32 night-light status, including mode, LED state, brightness, threshold, and light level
- `esp32_led_on()` — turns the LED on at full brightness and switches to manual mode
- `esp32_led_off()` — turns the LED off and switches to manual mode
- `esp32_get_threshold()` — reads the current auto-mode threshold
- `esp32_set_threshold(value)` — updates the auto-mode threshold
- `esp32_set_mode(mode)` — switches between `auto` and `manual` mode
- `esp32_get_brightness()` — reads the current brightness
- `esp32_set_brightness(value)` — sets the brightness directly

These tools are useful because the ESP32 firmware exposes matching HTTP endpoints such as `/api/status`, `/api/led/on`, `/api/brightness`, and `/api/threshold`.

## Using OpenAI models instead

The client uses the OpenAI SDK interface, which means it can also be adapted to work with OpenAI-based models if you later decide to switch away from the local setup.

In that case, you would typically:
- change the base URL to the desired provider endpoint
- update the model name
- provide the relevant API credentials

That said, the current version intentionally uses a local llama.cpp-backed model because the goal was to keep the setup self-contained, private, and completely runnable without depending on a cloud API.

## File overview

- `server.py` — defines the MCP tools and exposes them to the client
- `client.py` — connects the local model to the MCP server and handles tool-calling loops
- `pyproject.toml` — Python dependencies for the project
- `README.md` — this file

## Notes and next steps

This project is a useful starting point for:
- local AI agents that use tools
- home automation experiments
- ESP32-based device control through natural language
- lightweight MCP demonstrations that run without cloud services
