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
cat <<EOF >> /etc/modules
libcomposite
dwc2
g_hid
EOF

#Change usb controller to peripheral mode
sed -i '/\[cm5\]/,/^$/s/dr_mode=host/dr_mode=peripheral/' /boot/firmware/config.txt

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
