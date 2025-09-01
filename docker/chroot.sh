#!/bin/bash

if [ -z "$PI_ROOT" ]; then
	exit 1
fi

mkdir -vp /work/chroot
MOUNT_DIR=/work/chroot
mount "$PI_ROOT" "$MOUNT_DIR"

cp -vr /src/* $MOUNT_DIR/opt/pibot/
ln -s $MOUNT_DIR/opt/pibot/services/*.service $MOUNT_DIR/etc/systemd/system/
ln -s $MOUNT_DIR/opt/pibot/services/*.sh/ $MOUNT_DIR/usr/bin/

mount -v --bind /dev $MOUNT_DIR/dev
mount -v --bind /dev/pts $MOUNT_DIR/dev/pts
mount -v --bind /proc $MOUNT_DIR/proc
mount -v --bind /sys $MOUNT_DIR/sys

install() {
	# Install Requirements
	export DEBIAN_FRONTEND=noninteractive
	apt update -q
	curl -fsSL https://deb.nodesource.com/setup_20.x -o nodesource_setup.sh
	bash nodesource_setup.sh
	apt install -yq python3 python3-venv python3-pip tesseract-ocr git wget curl nodejs/nodistro ustreamer libgl1
	rm nodesource_setup.sh

	# Enable USB Gadget
	groupadd -g 500 pibot
	useradd -u 500 -g 500 -m -s /bin/bash pibot
	usermod -aG video pibot

	cat <<EOF >>/etc/modules
libcomposite
dwc2
g_hid
EOF

	chown -vR pibot:pibot /opt/pibot

	# Create venv for python backend
	sudo -u pibot python3 -m venv /opt/pibot/switch-control/venv
	sudo -u pibot /opt/pibot/switch-control/venv/bin/pip install --upgrade pip
	sudo -u pibot /opt/pibot/switch-control/venv/bin/pip install -r /opt/switch-control/requirements.txt

	# NPM Build
	npm install --prefix /opt/pibot/webui
	npm run build --prefix /opt/pibot/webui

	# Enable Services
	systemctl daemon-reload
	systemctl enable firstboot usbgadget pibot-backend pibot-frontend npmbuild ustreamer
}

chroot $MOUNT_DIR /bin/bash -c "$(declare -f install); install"

exit_chroot() {
	umount $MOUNT_DIR/dev
	umount $MOUNT_DIR/dev/pts
	umount $MOUNT_DIR/proc
	umount $MOUNT_DIR/sys
}

trap exit_chroot EXIT INT
