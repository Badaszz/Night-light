import asyncio
import json
from openai import OpenAI
from mcp import ClientSession, StdioServerParameters
from mcp.client.stdio import stdio_client

# --- Config ---
LLAMA_BASE_URL = "http://localhost:8080/v1"
MODEL_NAME = "qwen"  


async def run_agent(user_message: str):
    # Spawn the MCP server and connect to it
    server_params = StdioServerParameters(
        command="uv",
        args=["run", "server.py"],
    )

    async with stdio_client(server_params) as (read, write):
        async with ClientSession(read, write) as session:
            # Initialize the MCP session
            await session.initialize()

            # Fetch available tools from the server
            tools_response = await session.list_tools()
            
            # Convert MCP tool format -> OpenAI function calling format
            openai_tools = []
            for tool in tools_response.tools:
                openai_tools.append({
                    "type": "function",
                    "function": {
                        "name": tool.name,
                        "description": tool.description,
                        "parameters": tool.inputSchema,
                    }
                })

            print(f"[*] Available tools: {[t.name for t in tools_response.tools]}")

            # Set up OpenAI client pointing at your local llama-server
            llm = OpenAI(base_url=LLAMA_BASE_URL, api_key="none")

            # Build the conversation history
            messages = [
                {
                    "role": "system",
                    "content": "You are a helpful assistant with access to tools. "
                               "Use tools when needed to answer the user's question accurately."
                },
                {
                    "role": "user",
                    "content": user_message
                }
            ]

            print(f"[*] User: {user_message}\n")

            # Agentic loop — keep going until LLM stops calling tools
            while True:
                response = llm.chat.completions.create(
                    model=MODEL_NAME,
                    messages=messages,
                    tools=openai_tools,
                    tool_choice="auto",  # let the model decide
                    max_tokens=512,
                )

                assistant_msg = response.choices[0].message
                
                # Add assistant's response to history
                messages.append(assistant_msg.model_dump(exclude_unset=False))

                # Check if the model wants to call a tool
                if assistant_msg.tool_calls:
                    for tool_call in assistant_msg.tool_calls:
                        tool_name = tool_call.function.name
                        tool_args = json.loads(tool_call.function.arguments)

                        print(f"[*] LLM calling tool: {tool_name}({tool_args})")

                        # Execute the tool via MCP
                        result = await session.call_tool(tool_name, tool_args)
                        tool_result_text = result.content[0].text if result.content else "No result"

                        print(f"[*] Tool result: {tool_result_text}\n")

                        # Feed the tool result back into conversation
                        messages.append({
                            "role": "tool",
                            "tool_call_id": tool_call.id,
                            "content": tool_result_text,
                        })
                else:
                    # No tool calls = LLM is done, print final answer
                    print(f"[Assistant]: {assistant_msg.content}")
                    break


if __name__ == "__main__":
    while True:
        user_input = input("\nEnter a prompt (or 'quit' to exit): ").strip()
        if user_input.lower() == 'quit':
            break
        if not user_input:
            continue
        print("=" * 50)
        asyncio.run(run_agent(user_input))
        print()