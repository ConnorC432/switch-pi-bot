#!/bin/bash

rm /etc/nginx/default
ln -sf /opt/switch-pi-bot/nginx /etc/nginx/sites-enabled/pibot
systemctl restart nginx

sed -i 's/^dtoverlay=dwc2,dr_mode=host/dtoverlay=dwc2,dr_mode=peripheral/' /boot/firmware/config.txt

npm install --prefix /opt/switch-pi-bot/src/webui
npm run build --prefix /opt/switch-pi-bot/src/webui
