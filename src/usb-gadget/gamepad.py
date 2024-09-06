import time


class Gamepad:
    #Button Hex Values
    BUTTON_HEX = {
        'Y': (0x01, 0x00),
        'B': (0x02, 0x00),
        'A': (0x04, 0x00),
        'X': (0x08, 0x00),
        'L': (0x10, 0x00),
        'R': (0x20, 0x00),
        'ZL': (0x40, 0x00),
        'ZR': (0x80, 0x00),
        'Minus': (0x00, 0x01),
        'Plus': (0x00, 0x02),
        'L3': (0x00, 0x04),
        'R3': (0x00, 0x08),
        'Power': (0x00, 0x10),
        'Capture': (0x00, 0x20)
    }

    #DPad Hex Values
    DPAD_HEX = {
        'right': 0x01,
        'left': 0x05,
        'down': 0x03,
        'up': 0x07,
        'center': 0x00
    }

    #Stick Axes Map
    AXES_HEX = {
        'center': (0x80, 0x80),
        'N': (0x80, 0x00),
        'NE': (0xFF, 0x00),
        'E': (0xFF, 0x80),
        'SE': (0xFF, 0xFF),
        'S': (0x80, 0xFF),
        'SW': (0x00, 0xFF),
        'W': (0x80, 0x80),
        'NW': (0x00, 0x00),
    }

    def __init__(self, device_path="/dev/hidg0"):
        self.device_path = device_path
        self.default_state = bytearray([0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x00])

    #Write to USB Gadget block device
    def _write(self, data):
        with open(self.device_path, "wb") as device:
            device.write(data)

    def press_button(self, hold_time, *buttons):
        byte_1 = 0x00
        byte_2 = 0x00

        for button in buttons:
            #Get hex values from dictionary
            if button in self.BUTTON_HEX:
                hex_1, hex_2 = self.BUTTON_HEX[button]

                #Add to combined byte with bitwise OR
                byte_1 |= hex_1
                byte_2 |= hex_2

        #Ensure bytes are within limit with bitwise AND
        byte_1 &= 0xFF
        byte_2 &= 0xFF

        #Add button hex to report hex
        report_hex = self.default_state
        report_hex[0] = byte_1
        report_hex[1] = byte_2

        #Write hex to block device
        self._write(report_hex)
        time.sleep(hold_time)
        self._write(self.default_state)

    def press_dpad(self, hold_time, direction):
        report_hex = self.default_state

        if direction in self.DPAD_HEX:
            #Add DPad hex
            report_hex[2] = self.DPAD_HEX[direction]

            self._write(report_hex)
            time.sleep(hold_time)
            self._write(self.default_state)

    def move_left_stick(self, hold_time, direction):
        report_hex = self.default_state

        if direction in self.AXES_HEX:
            x_hex, y_hex = self.AXES_HEX[direction]

            report_hex[4] = x_hex
            report_hex[5] = y_hex

            self._write(report_hex)
            time.sleep(hold_time)
            self._write(self.default_state)

    def move_right_stick(self, hold_time, direction):
        report_hex = self.default_state

        if direction in self.AXES_HEX:
            x_hex, y_hex = self.AXES_HEX[direction]

            report_hex[6] = x_hex
            report_hex[7] = y_hex

            self._write(report_hex)
            time.sleep(hold_time)
            self._write(self.default_state)


if __name__ == "__main__":
    gamepad = Gamepad("/dev/hidg0")