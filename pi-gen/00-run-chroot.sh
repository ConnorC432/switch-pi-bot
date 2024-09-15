mkdir -p ${ROOTFS_DIR}/proc
mkdir -p ${ROOTFS_DIR}/dev
mkdir -p ${ROOTFS_DIR}/sys
mkdir -p ${ROOTFS_DIR}/run
mkdir -p ${ROOTFS_DIR}/tmp

mount -o bind /proc ${ROOTFS_DIR}/proc
mount -o bind /dev ${ROOTFS_DIR}/dev
mount -o bind /sys ${ROOTFS_DIR}/sys
mount -o bind /run ${ROOTFS_DIR}/run
mount -o bind /tmp ${ROOTFS_DIR}/tmp

apt update
apt upgrade -y
apt install -y python3 python3-venv python3-pip tesseract-ocr git wget curl
apt purge nodejs npm nodejs-legacy libnode72
apt autoremove
curl -fsSL https://deb.nodesource.com/setup_20.x -o nodesource_setup.sh
bash nodesource_setup.sh
apt-get install -fy nodejs/nodistro
rm nodesource_setup.sh

#Clone Repo
echo "Cloning Git Repository"
git clone https://github.com/ConnorC432/switch-pi-bot.git /opt/switch-pi-bot

#Create User/Group
echo "Creating User and Group for Services"
groupadd -g 500 pibot
useradd -u 500 -g 500 -m -s /bin/bash pibot

#load kernel modules
#echo "Enabling USB HID Gadget Mode"
#cat <<EOF >> /etc/modules
#libcomposite
#dwc2
#g_hid
#EOF

#Change usb controller to peripheral mode
#sed -i '/\[cm5\]/,/^$/s/dr_mode=host/dr_mode=peripheral/' /boot/firmware/config.txt

#Set Permissions
chown -vR pibot:pibot /opt/switch-pi-bot
#chmod -vR 770 /opt/switch-pi-bot

#Create .venv for python backend
echo "Creating Virtual Environment for Python backend"
python3 -m venv /opt/switch-pi-bot/src/switch-control/.venv
/opt/switch-pi-bot/src/switch-control/.venv/bin/pip install --upgrade pip
/opt/switch-pi-bot/src/switch-control/.venv/bin/pip install -r /opt/switch-pi-bot/requirements.txt

#NPM Build
echo "Building Next.JS frontend"
npm --prefix /opt/switch-pi-bot/src/webui install
npm --prefix /opt/switch-pi-bot/src/webui run build

#Systemd service files
echo "Importing Systemd service files"
cp /opt/switch-pi-bot/services/*.service /etc/systemd/system
/usr/bin/systemctl daemon-reload
/usr/bin/systemctl enable usbgadget.service pibot-backend.service pibot-frontend.service

umount ${ROOTFS_DIR}/proc
umount ${ROOTFS_DIR}/dev
umount ${ROOTFS_DIR}/sys
umount ${ROOTFS_DIR}/run
umount ${ROOTFS_DIR}/tmp