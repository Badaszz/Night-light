from mcp.server.fastmcp import FastMCP

## Initialize the FastMCP server
mcp = FastMCP("local-tools")

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

if __name__ == "__main__":
    mcp.run(transport="stdio")