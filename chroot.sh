# Install Requirements
apt update
curl -fsSL https://deb.nodesource.com/setup_20.x -o nodesource_setup.sh
bash nodesource_setup.sh
apt install -y python3 python3-venv python3-pip tesseract-ocr git wget curl nodejs/nodistro nginx
rm nodesource_setup.sh

# Enable USB Gadget
groupadd -g 500 pibot
useradd -u 500 -g 500 -m -s /bin/bash pibot

cat <<EOF >>/etc/modules
libcomposite
dwc2
g_hid
EOF

chown -vR pibot:pibot /opt/switch-pi-bot

# Create venv for python backend
sudo -u pibot python3 -m venv /opt/switch-control/venv
sudo -u pibot /opt/switch-control/venv/bin/pip install --upgrade pip
sudo -u pibot /opt/switch-control/venv/bin/pip install -r /opt/switch-control/requirements.txt

# NPM Build
npm install --prefix /opt/webui
npm run build --prefix /opt/webui

# Enable Services
systemctl daemon-reload
systemctl enable firstboot usbgadget pibot-backend pibot-frontend
