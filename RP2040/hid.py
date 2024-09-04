import board
import usb_hid

HID_REPORT_DESCRIPTOR = bytes([
    0x05, 0x01,  # USAGE_PAGE (Generic Desktop)
    0x09, 0x05,  # USAGE (Game Pad)
    0xA1, 0x01,  # COLLECTION (Application)

    # Axes (X, Y, Z, Rx, Ry, Rz)
    0x05, 0x01,  # USAGE_PAGE (Generic Desktop)
    0x09, 0x30,  # USAGE (X)
    0x09, 0x31,  # USAGE (Y)
    0x09, 0x32,  # USAGE (Z)
    0x09, 0x33,  # USAGE (Rx)
    0x09, 0x34,  # USAGE (Ry)
    0x09, 0x35,  # USAGE (Rz)
    0x16, 0x00, 0x80,  # LOGICAL_MINIMUM (-32768)
    0x26, 0xFF, 0x7F,  # LOGICAL_MAXIMUM (32767)
    0x75, 0x10,  # REPORT_SIZE (16)
    0x95, 0x06,  # REPORT_COUNT (6)
    0x81, 0x02,  # INPUT (Data,Var,Abs)

    # D-Pad (Hat switch)
    0x05, 0x01,  # USAGE_PAGE (Generic Desktop)
    0x09, 0x39,  # USAGE (Hat switch)
    0x15, 0x00,  # LOGICAL_MINIMUM (0)
    0x25, 0x07,  # LOGICAL_MAXIMUM (7)
    0x35, 0x00,  # PHYSICAL_MINIMUM (0)
    0x46, 0x3B, 0x01,  # PHYSICAL_MAXIMUM (315)
    0x65, 0x14,  # UNIT (Eng Rot:Angular Pos)
    0x75, 0x04,  # REPORT_SIZE (4)
    0x95, 0x01,  # REPORT_COUNT (1)
    0x81, 0x42,  # INPUT (Data,Var,Abs,Null)

    # Padding for alignment
    0x75, 0x04,  # REPORT_SIZE (4)
    0x95, 0x01,  # REPORT_COUNT (1)
    0x81, 0x01,  # INPUT (Const,Ary,Abs)

    # Buttons (14 buttons)
    0x05, 0x09,  # USAGE_PAGE (Button)
    0x19, 0x01,  # USAGE_MINIMUM (Button 1)
    0x29, 0x0E,  # USAGE_MAXIMUM (Button 14)
    0x15, 0x00,  # LOGICAL_MINIMUM (0)
    0x25, 0x01,  # LOGICAL_MAXIMUM (1)
    0x75, 0x01,  # REPORT_SIZE (1)
    0x95, 0x0E,  # REPORT_COUNT (14)
    0x81, 0x02,  # INPUT (Data,Var,Abs)

    # Padding for alignment
    0x75, 0x02,  # REPORT_SIZE (2)
    0x95, 0x01,  # REPORT_COUNT (1)
    0x81, 0x01,  # INPUT (Cnst,Ary,Abs)

    0xC0  # END_COLLECTION
])


gamepad = usb_hid.Device(
    report_descriptor=HID_REPORT_DESCRIPTOR,
    usage_page=0x01
    usage=0x05
    report_ids=()
)