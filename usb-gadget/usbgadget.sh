#!/bin/bash

if [[ $EUID -ne 0 ]]; then
  echo "This script must be run as root"
  exit 1
fi

set -e
set -u

modprobe libcomposite
dtoverlay dwc2

mkdir -p /sys/kernel/config/usb_gadget/
cd /sys/kernel/config/usb_gadget/ || exit

mkdir -p gamepad
cd gamepad

echo 0xf0d > idVendor
echo 0x92 > idProduct
echo 0x0100 > bcdDevice
echo 0x0200 > bcdUSB
echo 0x03 > bDeviceClass
echo 0x00 > bDeviceSubClass
echo 0x00 > bDeviceProtocol
echo 0x40 > bMaxPacketSize0

#Strings
mkdir -p strings/0x409
echo "0123456789" > strings/0x409/serialnumber
echo "Hori Co., Ltd" > strings/0x409/manufacturer
echo "Pokkén Tournament Controller" > strings/0x409/product

#Config
mkdir -p configs/c.1/strings/0x409
echo "Config 1" > configs/c.1/strings/0x409/configuration
echo 0xFA > configs/c.1/MaxPower

#HID Function
mkdir -p functions/hid.usb0
echo 1 > functions/hid.usb0/protocol
echo 1 > functions/hid.usb0/subclass
echo 64 > functions/hid.usb0/report_length

# HID Report Descriptor
echo -ne '\x05\x01\x09\x05\xa1\x01' \
         '\x09\x30\x09\x33\x15\x81\x25\x7f\x75\x08\x95\x02\x81\x02' \
         '\x09\x31\x09\x34\x15\x81\x25\x7f\x75\x08\x95\x02\x81\x02' \
         '\x09\x32\x09\x35\x15\x81\x25\x7f\x75\x01\x95\x02\x81\x02' \
         '\x05\x09\x19\x01\x29\x3f\x15\x00\x25\x01\x75\x01\x95\x3f\x81\x02' \
         '\x05\x01\x09\x20\x25\x01\x75\x01\x95\x02\x81\x02\xc0' > functions/hid.usb0/report_desc

ln -s functions/hid.usb0 configs/c.1/

ls /sys/class/udc > UDC

chmod 777 /dev/hidg0
