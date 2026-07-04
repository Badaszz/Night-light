import os
import requests
from dotenv import load_dotenv
from mcp.server.fastmcp import FastMCP

# Load environment variables from .env
load_dotenv()

## Initialize the FastMCP server
mcp = FastMCP("local-tools")

# --- ESP32 config ---
# Put ESP32_IP=<your esp32's ip> in a .env file next to this script
ESP32_IP = os.getenv("ESP32_IP")
ESP32_BASE_URL = f"http://{ESP32_IP}"
REQUEST_TIMEOUT = 5  # seconds - don't hang forever if the ESP32 is offline


def _esp32_get(path: str, params: dict = None) -> str:
    """Shared GET helper for all ESP32 endpoints. Returns raw response
    text on success, or a readable error string on failure so the LLM
    can react sensibly instead of crashing the agent loop."""
    if not ESP32_IP:
        return "Error: ESP32_IP is not set. Add ESP32_IP=<ip address> to your .env file."
    try:
        resp = requests.get(f"{ESP32_BASE_URL}{path}", params=params, timeout=REQUEST_TIMEOUT)
        resp.raise_for_status()
        return resp.text
    except requests.exceptions.ConnectionError:
        return f"Error: Could not reach ESP32 at {ESP32_BASE_URL}. Is it powered on and on the same network?"
    except requests.exceptions.Timeout:
        return f"Error: ESP32 at {ESP32_BASE_URL} did not respond in time."
    except requests.exceptions.RequestException as e:
        return f"Error contacting ESP32: {str(e)}"


def _esp32_post(path: str, data: dict) -> str:
    """Shared POST helper for ESP32 endpoints that accept a form value."""
    if not ESP32_IP:
        return "Error: ESP32_IP is not set. Add ESP32_IP=<ip address> to your .env file."
    try:
        resp = requests.post(f"{ESP32_BASE_URL}{path}", data=data, timeout=REQUEST_TIMEOUT)
        resp.raise_for_status()
        return resp.text
    except requests.exceptions.ConnectionError:
        return f"Error: Could not reach ESP32 at {ESP32_BASE_URL}. Is it powered on and on the same network?"
    except requests.exceptions.Timeout:
        return f"Error: ESP32 at {ESP32_BASE_URL} did not respond in time."
    except requests.exceptions.RequestException as e:
        return f"Error contacting ESP32: {str(e)}"

@mcp.tool()
def hello_world():
    """
    A simple hello world tool.
    """
    return "Hello, World!"

@mcp.tool()
def calculator(operation: str, a: float, b: float) -> str:
    """Perform basic arithmetic operations

    Args:
        operation (One of 'add', 'subtract', 'multiply', 'divide'): operation to perform
        a (float): First number
        b (float): Second number

    Returns:
        str: Returns the value of the result as a string
    """
    if operation == "add":
        return str(a + b)
    elif operation == "subtract":
        return str(a - b)
    elif operation == "multiply":
        return str(a * b)
    elif operation == "divide":
        if b == 0:
            return "Error: Division by zero"
        return str(a / b)
    else:
        return "Error: Invalid operation. Supported operations are 'add', 'subtract', 'multiply', 'divide'."
    
@mcp.tool()
def funny_calculator(operation: str, a: float, b: float) -> str:
    """Perform basic arithmetic operations with a funny twist

    Args:
        operation (One of 'add', 'subtract', 'multiply', 'divide'): operation to perform
        a (float): First number
        b (float): Second number

    Returns:
        str: Returns the value of the result as a string with a funny message
    """
    if operation == "add":
        return f"The sum of {a} and {b} is {a + b}. That's a lot of math!"
    elif operation == "subtract":
        return f"The difference between {a} and {b} is {a - b}. Math is fun!"
    elif operation == "multiply":
        return f"The product of {a} and {b} is {a * b}. Multiplication is magical!"
    elif operation == "divide":
        if b == 0:
            return "Error: Division by zero. You can't divide by zero, that's just not allowed!"
        return f"The quotient of {a} and {b} is {a / b}. Division is divisive!"
    else:
        return "Error: Invalid operation. Supported operations are 'add', 'subtract', 'multiply', 'divide'. Don't try to be fancy!"
    
@mcp.tool()
def read_file(file_path: str) -> str:
    """Read the contents of a text file

    Args:
        file_path (str): Absolute or relative file path

    Returns:
        str: the contents of the file as a string
    """
    try:
        with open(file_path, 'r') as file:
            return file.read()
    except FileNotFoundError:
        return f"Error: The file '{file_path}' was not found."
    except Exception as e:
        return f"An error occurred while reading the file: {str(e)}"
    
@mcp.tool()
def get_system_info() -> str:
    """Returns basic info about the current system

    Returns:
        str: basic info about system
    """
    import platform
    
    return f"System: {platform.system()}, Node Name: {platform.node()}, Release: {platform.release()}, Version: {platform.version()}, Machine: {platform.machine()}, Processor: {platform.processor()}"

@mcp.tool()
def esp32_get_status() -> str:
    """Get the full current status of the ESP32 night light: mode
    (auto/manual), whether the LED is on, current brightness (0-255),
    the auto-mode threshold (0-4095), and the raw light sensor reading.

    Returns:
        str: JSON string with keys mode, led, brightness, threshold, lightLevel
    """
    return _esp32_get("/api/status")


@mcp.tool()
def esp32_led_on() -> str:
    """Turn the night light's LED on at full brightness. This switches
    the ESP32 to manual mode, overriding automatic light-based control.

    Returns:
        str: JSON string confirming the new status
    """
    return _esp32_get("/api/led/on")


@mcp.tool()
def esp32_led_off() -> str:
    """Turn the night light's LED off. This switches the ESP32 to
    manual mode, overriding automatic light-based control.

    Returns:
        str: JSON string confirming the new status
    """
    return _esp32_get("/api/led/off")


@mcp.tool()
def esp32_get_threshold() -> str:
    """Get the current auto-mode light threshold. This is the raw LDR
    reading (0-4095) below which the light is considered 'dark enough'
    for the LED to turn on automatically.

    Returns:
        str: JSON string with the threshold value
    """
    return _esp32_get("/api/threshold")


@mcp.tool()
def esp32_set_threshold(value: int) -> str:
    """Set the auto-mode light threshold. Higher values make the light
    turn on earlier (at brighter ambient light); lower values mean it
    only turns on once the room is quite dark.

    Args:
        value (int): New threshold, 0-4095

    Returns:
        str: JSON string confirming the new threshold
    """
    return _esp32_post("/api/threshold", {"value": value})


@mcp.tool()
def esp32_set_mode(mode: str) -> str:
    """Switch the ESP32 between automatic and manual light control.

    Args:
        mode (One of 'auto', 'manual'): 'auto' lets the LDR sensor decide
            brightness automatically. 'manual' holds the LED at whatever
            state it was last set to via esp32_led_on/off/esp32_set_brightness.

    Returns:
        str: JSON string confirming the new mode, or an error if the
             mode string wasn't 'auto' or 'manual'
    """
    mode = mode.lower().strip()
    if mode not in ("auto", "manual"):
        return "Error: mode must be 'auto' or 'manual'."
    return _esp32_get(f"/api/mode/{mode}")


@mcp.tool()
def esp32_get_brightness() -> str:
    """Get the ESP32 night light's current LED brightness.

    Returns:
        str: JSON string with the brightness value, 0-255
    """
    return _esp32_get("/api/brightness")


@mcp.tool()
def esp32_set_brightness(value: int) -> str:
    """Set the ESP32 night light's LED brightness directly. This
    switches the ESP32 to manual mode, overriding automatic control.

    Args:
        value (int): Brightness, 0-255 (0 = off, 255 = full brightness)

    Returns:
        str: JSON string confirming the new brightness
    """
    return _esp32_post("/api/brightness", {"value": value})


if __name__ == "__main__":
    mcp.run(transport="stdio")