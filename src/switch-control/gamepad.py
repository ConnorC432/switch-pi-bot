import os
import time


def find_gadgets():
	gadgets = []

	for gadget_device in os.listdir("/dev"):
		if gadget_device.startswith("hidg"):
			# Test device
			with open(gadget_device, "wb") as hid_device:
				try:
					hid_device.write(bytearray([0x00, 0x00, 0x08, 0x80, 0x80, 0x80, 0x80, 0x00]))

				except Exception as e:
					print(f"Cant write to block device: {e}")
					continue

				gadgets.append(f"/dev/{gadget_device}")

	return gadgets


class Gamepad:
	# Button Hex Values
	BUTTON_HEX = {
		"Y": (0x01, 0x00),
		"B": (0x02, 0x00),
		"A": (0x04, 0x00),
		"X": (0x08, 0x00),
		"L": (0x10, 0x00),
		"R": (0x20, 0x00),
		"ZL": (0x40, 0x00),
		"ZR": (0x80, 0x00),
		"Minus": (0x00, 0x01),
		"Plus": (0x00, 0x02),
		"L3": (0x00, 0x04),
		"R3": (0x00, 0x08),
		"Power": (0x00, 0x10),
		"Capture": (0x00, 0x20)
	}

	# DPad Hex Values
	DPAD_HEX = {
		"center": 0x08,
		"N": 0x00,
		"NE": 0x01,
		"E": 0x02,
		"SE": 0x03,
		"S": 0x04,
		"SW": 0x05,
		"W": 0x06,
		"NW": 0x07
	}

	# Stick Axes Map
	AXES_HEX = {
		"center": (0x80, 0x80),
		"N": (0x80, 0x00),
		"NE": (0xFF, 0x00),
		"E": (0xFF, 0x80),
		"SE": (0xFF, 0xFF),
		"S": (0x80, 0xFF),
		"SW": (0x00, 0xFF),
		"W": (0x00, 0x80),
		"NW": (0x00, 0x00)
	}

	DEFAULT_STATE = bytearray([0x00, 0x00, 0x08, 0x80, 0x80, 0x80, 0x80, 0x00])

	def __init__(self, device_path="/dev/hidg0"):
		self.device_path = device_path

	# Write to USB Gadget block device
	def _write(self, data):
		with open(self.device_path, "wb") as device:
			try:
				device.write(data)

			except Exception as e:
				print(f"Cant write to block device: {e}")

	def _clear(self):
		with open(self.device_path, "wb") as device:
			try:
				print(self.DEFAULT_STATE)
				device.write(bytearray([0x00, 0x00, 0x08, 0x80, 0x80, 0x80, 0x80, 0x00]))
				return bytearray([0x00, 0x00, 0x08, 0x80, 0x80, 0x80, 0x80, 0x00])

			except Exception as e:
				print(f"Cant write to block device: {e}")

	def press_button(self, hold_time, *buttons):
		report_hex = bytearray(self.DEFAULT_STATE)
		byte_1 = 0x00
		byte_2 = 0x00

		for button in buttons:
			# Get hex values from dictionary
			if button in self.BUTTON_HEX:
				hex_1, hex_2 = self.BUTTON_HEX[button]

				# Add to combined byte with bitwise OR
				byte_1 |= hex_1
				byte_2 |= hex_2

		# Ensure bytes are within limit with bitwise AND
		byte_1 &= 0xFF
		byte_2 &= 0xFF

		# Add button hex to report hex
		report_hex[0] = byte_1
		report_hex[1] = byte_2
		print(f"Sending to HID Device: {report_hex}")

		# Write hex to block device
		self._write(report_hex)
		time.sleep(hold_time)
		report_hex = self._clear()
		print(f"Report Hex Array: {report_hex}")

	def press_dpad(self, hold_time, direction):
		report_hex = bytearray(self.DEFAULT_STATE)

		if direction in self.DPAD_HEX:
			# Add DPad hex
			report_hex[2] = self.DPAD_HEX[direction]

			print(f"Sending to HID Device: {report_hex}")

			self._write(report_hex)
			time.sleep(hold_time)
			report_hex = self._clear()
			print(f"Report Hex Array: {report_hex}")

	def move_left_stick(self, hold_time, direction):
		report_hex = bytearray(self.DEFAULT_STATE)

		# Set X and Y axes to hex value
		if direction in self.AXES_HEX:
			x_hex, y_hex = self.AXES_HEX[direction]

			report_hex[3] = x_hex
			report_hex[4] = y_hex

			print(f"Sending to HID Device: {report_hex}")

			self._write(report_hex)
			time.sleep(hold_time)
			report_hex = self._clear()
			print(f"Report Hex Array: {report_hex}")

	def move_right_stick(self, hold_time, direction):
		report_hex = bytearray(self.DEFAULT_STATE)

		# Set Rx and Ry axes to hex value
		if direction in self.AXES_HEX:
			x_hex, y_hex = self.AXES_HEX[direction]

			report_hex[5] = x_hex
			report_hex[6] = y_hex

			print(f"Sending to HID Device: {report_hex}")

			self._write(report_hex)
			time.sleep(hold_time)
			report_hex = self._clear()
			print(f"Report Hex Array: {report_hex}")


if __name__ == "__main__":
	gamepad = Gamepad("/dev/hidg0")
