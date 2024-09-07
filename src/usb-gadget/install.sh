#!/bin/bash

if [[ $EUID -ne 0 ]]; then
  echo "This script must be run as root"
  exit 1
fi

#load kernel modules
cat <<EOF >> /etc/modules
libcomposite
dwc2
g_hid
EOF

#Change usb controller to peripheral mode
sed -i '/\[cm5\]/,/^$/s/dr_mode=host/dr_mode=peripheral/' /boot/firmware/config.txt

#USB Gadget script
curl -o /usr/bin/usbgadget.sh https://raw.githubusercontent.com/ConnorC432/RPi-Switch-Bot/main/src/usb-gadget/usbgadget.sh

#Initialise usb gadget on boot
sed -i '/exit 0/i /usr/bin/usbgadget.sh &' /etc/rc.local

reboot now