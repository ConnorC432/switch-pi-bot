#!/bin/bash

if [ -z "$PI_ROOT" ]; then
	exit 1
fi

mkdir -vp /work/chroot
MOUNT_DIR=/work/chroot
mount "$PI_ROOT" "$MOUNT_DIR"

mkdir -vp $MOUNT_DIR/opt/switch-pi-bot
cp -r /src/* $MOUNT_DIR/opt/switch-pi-bot

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

	# Link Systemd service files
	ln -sf /opt/switch-pi-bot/services/* /etc/systemd/system/

	# Enable USB Gadget
	groupadd -g 500 pibot
	useradd -u 500 -g 500 -m -s /bin/bash pibot

	cat <<EOF >>/etc/modules
libcomposite
dwc2
g_hid
EOF

	ln -sf /opt/switch-pi-bot/firstboot.sh /etc/init.d/firstboot.sh
	chown -vR pibot:pibot /opt/switch-pi-bot

	# Create .venv for python backend
	python3 -m venv /opt/switch-pi-bot/src/switch-control/.venv
	/opt/switch-pi-bot/src/switch-control/.venv/bin/pip install --upgrade pip
	/opt/switch-pi-bot/src/switch-control/.venv/bin/pip install -r /opt/switch-pi-bot/requirements.txt

	# NPM Build
	npm --prefix /opt/switch-pi-bot/src/webui install
	npm --prefix /opt/switch-pi-bot/src/webui run build

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
