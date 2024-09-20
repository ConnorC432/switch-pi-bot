#!/bin/bash

if [ -z "$PI_ROOT" ]; then
	exit 1
fi

mkdir -vp /work/chroot
MOUNT_DIR=/work/chroot
mount "$PI_ROOT" "$MOUNT_DIR"

cp -vr /src/src/* $MOUNT_DIR/opt/
cp -v /src/services/*.service $MOUNT_DIR/etc/systemd/system/
cp -v /src/services/*.sh $MOUNT_DIR/usr/bin/
cp -v /src/nginx /opt/nginx

mount -v --bind /dev $MOUNT_DIR/dev
mount -v --bind /dev/pts $MOUNT_DIR/dev/pts
mount -v --bind /proc $MOUNT_DIR/proc
mount -v --bind /sys $MOUNT_DIR/sys

install() {
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
}

chroot $MOUNT_DIR /bin/bash -c "$(declare -f install); install"

exit_chroot() {
	umount $MOUNT_DIR/dev
	umount $MOUNT_DIR/dev/pts
	umount $MOUNT_DIR/proc
	umount $MOUNT_DIR/sys
}

trap exit_chroot EXIT INT
