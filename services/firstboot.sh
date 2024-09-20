#!/bin/bash

sudo setcap 'cap_net_bind_service=+ep' /usr/bin/node

sed -i 's/^dtoverlay=dwc2,dr_mode=host/dtoverlay=dwc2,dr_mode=peripheral/' /boot/firmware/config.txt

npm install --prefix /opt/webui
npm run build --prefix /opt/webui
