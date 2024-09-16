#!/bin/bash

ln -sf /opt/switch-pi-bot/nginx /etc/nginx/sites-enabled/pibot
systemctl restart nginx

sed -i 's/^dtoverlay=dwc2,dr_mode=host/dr_mode=peripheral: \[cm5\]/' /boot/firmware/config.txt

rm /etc/init.d/firstboot.sh
