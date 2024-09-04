import asyncio
import socket
from machine import Pin
import network
import ujson


led = Pin(25, Pin.OUT)


def connect_wifi():
    try:
        with open('config.json') as f:
            config = ujson.load(f)
            ssid = config['ssid']
            password = config['password']
        if not ssid or not password:
            raise ValueError('Please provide ssid and password')

        wlan = network.WLAN(network.STA_IF)
        wlan.active(True)
        wlan.connect(ssid, password)

        print('Connecting to network...')
        while not wlan.isconnected():
            pass
        print("Connected to Wifi")
        print('Network config:', wlan.ifconfig())

    except Exception as e:
        print("Failed to connect to network:", e)


async def handle_request(request):
    led.on()
    data = request.decode('utf-8')
    try:
        command = ujson.loads(data)
        action = command.get('action')
        if action == 'press':
            print("Button pressed")
            led.value(1)
        elif action == 'release':
            print("Button released")
            led.value(0)
    except Exception as e:
        print("Error", e)
    finally:
        led.off()


async def start_server():
    addr = socket.getaddrinfo('0.0.0.0', 80)[0][-1]
    s = socket.socket()
    s.bind(addr)
    s.listen(1)
    print("Listening on ", addr)

    while True:
        cl, addr = s.accept()
        print("Accepted connection from ", addr)
        request = cl.recv(1024)
        cl.send('HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n')
        cl.send('Received\n')
        await handle_request(request)
        cl.close()


async def main():
    connect_wifi()
    await start_server()


asyncio.run(main())
