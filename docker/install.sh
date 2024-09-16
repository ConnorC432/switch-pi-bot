#!/bin/bash

# Install Requirements
apt update
apt upgrade -y
curl -fsSL https://deb.nodesource.com/setup_20.x -o nodesource_setup.sh
bash nodesource_setup.sh
apt install -y python3 python3-venv python3-pip tesseract-ocr git wget curl nodejs/nodistro nginx
rm nodesource_setup.sh

# Move Relevant Source Code
cp /src/services/*.service /etc/systemd/system/
mkdir -p /opt/switch-pi-bot
cp /src/* /opt/switch-pi-bot/

# Enable USB Gadget
groupadd -g 500 pibot
useradd -u 500 -g 500 -m -s /bin/bash pibot

cat <<EOF >>/etc/modules
libcomposite
dwc2
g_hid
EOF

sed -i '/\[cm5\]/,/^$/s/dr_mode=host/dr_mode=peripheral/' /boot/firmware/config.txt
chown -vR pibot:pibot /opt/switch-pi-bot

# Create .venv for python backend
python3 -m venv /opt/switch-pi-bot/src/switch-control/.venv
/opt/switch-pi-bot/src/switch-control/.venv/bin/pip install --upgrade pip
/opt/switch-pi-bot/src/switch-control/.venv/bin/pip install -r /opt/switch-pi-bot/requirements.txt

# NPM Build
npm --prefix /opt/switch-pi-bot/src/webui install
npm --prefix /opt/switch-pi-bot/src/webui run build

# NGINX
rm /etc/nginx/sites-enabled/default
cp /src/nginx /etc/nginx/sites-available/pibot
ln -s /etc/nginx/sites-available/pibot /etc/nginx/sites-enabled/pibot
systemctl restart nginx

# Enable Services
systemctl daemon-reload
systemctl enable usbgadget pibot-backend pibot-frontend
