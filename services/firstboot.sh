#!/bin/bash

rm /etc/nginx/default
ln -sf /opt/nginx /etc/nginx/sites-enabled/pibot
systemctl restart nginx

sed -i 's/^dtoverlay=dwc2,dr_mode=host/dtoverlay=dwc2,dr_mode=peripheral/' /boot/firmware/config.txt

npm install --prefix /opt/webui
npm run build --prefix /opt/webui
