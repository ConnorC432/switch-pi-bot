import asyncio
import websockets
import json
import requests
import sys

async def test_status_ws():
    uri = "ws://localhost:8080/program/status"

    try:
        async with websockets.connect(uri) as websocket:
            print("Connected to WebSocket")

            start_payload = {
                "programName": "Test",
                "displayName": "Test",
                "description": "Test Program",
                "category": "Tests",
                "settings": [
                    {
                        "argName": "waitTime",
                        "displayName": "Wait Time",
                        "value": 10
                    },
                    {
                        "argName": "testString",
                        "displayName": "Test String",
                        "value": "Testing..."
                    }
                ]
            }

            response = requests.post(
                "http://localhost:8080/program/start",
                headers={"Content-Type": "application/json"},
                data=json.dumps(start_payload)
            )

            print("/program/start response: ", response.text)
            if response.json().get("status") != "started":
                print("Failed to start program")
                sys.exit(1)

            found = False
            try:
                while True:
                    msg = await asyncio.wait_for(websocket.recv(), timeout=15)
                    print(msg)
                    if "{\"status\", \"started\"}" in msg:
                        found = True
                        break

            except asyncio.TimeoutError:
                print("Websocket timed out")

            if not found:
                print("WebSocket didn't contain expected response")
                sys.exit(1)
            else:
                print("WebSocket test passed")

    except Exception as e:
        print("WebSocket test failed: ", e)
        sys.exit(1)

if __name__ == "__main__":
    asyncio.run(test_status_ws())