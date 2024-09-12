#!/bin/bash

if [[ $EUID -ne 0 ]]; then
  echo "This script must be run as root"
  exit 1
fi

#Install required packages
apt update
apt upgrade -y
apt install -y python3 nodejs npm tesseract-ocr

#Create User/Group
groupadd -g 500 pibot
useradd -u 500 -g 500 -m -s /bin/bash pibot

#load kernel modules
cat <<EOF >> /etc/modules
libcomposite
dwc2
g_hid
EOF

#Change usb controller to peripheral mode
sed -i '/\[cm5\]/,/^$/s/dr_mode=host/dr_mode=peripheral/' /boot/firmware/config.txt

#Set Permissions
chown -R pibot:pibot /opt/switch-pi-bot
chmod -R 770 /opt/switch-pi-bot

#Create .venv for python backend
sudo -u pibot -g pibot bash <<EOF
python3 -m venv /opt/switch-pi-bot/src/switch-control/.venv
/opt/switch-pi-bot/src/switch-control/.venv/bin/pip install -r /opt/switch-pi-bot/requirements.txt
EOF

#NPM Build
sudo -u pibot -g pibot bash <<EOF
npm --prefix /opt/switch-pi-bot/src/webui/app install
npm --prefix /opt/switch-pi-bot/src/webui/app run build
EOF

#Systemd service files
cp /opt/switch-pi-bot/services/*.service /etc/systemd/system
systemctl daemon-reload
systemctl enable usbgadget.service pibot-backend.service pibot-frontend.service
