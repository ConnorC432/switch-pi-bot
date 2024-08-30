import serial
import time

ser = serial.Serial('/dev/serial0', 9600)

def send_data():
    while True:
        data = "Test\n"
        ser.write(data.encode())
        time.sleep(0.1)

try:
    send_data()
except KeyboardInterrupt:
    print('Interrupted')
finally:
    ser.close()
