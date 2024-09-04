from usb_hid import devices
from gamepad import Gamepad


device = Gamepad(devices)
device.click_buttons(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16)
device.move_joysticks(x=127, y=127, z=127, r_z=127)