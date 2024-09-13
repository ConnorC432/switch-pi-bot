#!/bin/bash

#Clone Repo
echo "Cloning Git Repository"
git clone https://github.com/ConnorC432/switch-pi-bot.git /opt/switch-pi-bot

#Create User/Group
echo "Creating User and Group for Services"
groupadd -g 500 pibot
useradd -u 500 -g 500 -m -s /bin/bash pibot

#load kernel modules
echo "Enabling USB HID Gadget Mode"
MODULES=$(cat <<EOF
# /etc/modules: kernel modules to load at boot time.
#
# This file contains the names of kernel modules that should be loaded
# at boot time, one per line. Lines beginning with "#" are ignored.
# Parameters can be specified after the module name.
libcomposite
dwc2
g_hid
EOF
)

echo "$MODULES" | tee /etc/modules > /dev/null

CONFIG=$(cat <<EOF
# For more options and information see
# http://rptl.io/configtxt
# Some settings may impact device functionality. See link above for details

# Uncomment some or all of these to enable the optional hardware interfaces
#dtparam=i2c_arm=on
#dtparam=i2s=on
#dtparam=spi=on

# Enable audio (loads snd_bcm2835)
dtparam=audio=on

# Additional overlays and parameters are documented
# /boot/firmware/overlays/README

# Automatically load overlays for detected cameras
camera_auto_detect=1

# Automatically load overlays for detected DSI displays
display_auto_detect=1

# Automatically load initramfs files, if found
auto_initramfs=1

# Enable DRM VC4 V3D driver
dtoverlay=vc4-kms-v3d
max_framebuffers=2

# Don't have the firmware create an initial video= setting in cmdline.txt.
# Use the kernel's default instead.
disable_fw_kms_setup=1

# Run in 64-bit mode
arm_64bit=1

# Disable compensation for displays with overscan
disable_overscan=1

# Run as fast as firmware / board allows
arm_boost=1

[cm4]
# Enable host mode on the 2711 built-in XHCI USB controller.
# This line should be removed if the legacy DWC2 controller is required
# (e.g. for USB device mode) or if USB support is not required.
otg_mode=1

[cm5]
dtoverlay=dwc2,dr_mode=peripheral

[all]
EOF
)

echo "$CONFIG" | tee /boot/firmware/config.txt > /dev/null

#Change usb controller to peripheral mode
#sed -i '/\[cm5\]/,/^$/s/dr_mode=host/dr_mode=peripheral/' /boot/firmware/config.txt

#Set Permissions
chown -vR pibot:pibot /opt/switch-pi-bot
#chmod -vR 770 /opt/switch-pi-bot

#Create .venv for python backend
echo "Creating Virtual Environment for Python backend"
python3 -m venv /opt/switch-pi-bot/src/switch-control/.venv
/opt/switch-pi-bot/src/switch-control/.venv/bin/pip install -r /opt/switch-pi-bot/requirements.txt

#NPM Build
echo "Building Next.JS frontend"
npm --prefix /opt/switch-pi-bot/src/webui install
npm --prefix /opt/switch-pi-bot/src/webui run build

#Systemd service files
echo "Importing Systemd service files"
cp /opt/switch-pi-bot/services/*.service /etc/systemd/system
systemctl daemon-reload
systemctl enable usbgadget.service pibot-backend.service pibot-frontend.service
