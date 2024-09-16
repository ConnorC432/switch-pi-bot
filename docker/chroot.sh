#!/bin/bash

if [ -z "$PI_ROOT" ]; then
	exit 1
fi

mkdir -vp /work/chroot
MOUNT_DIR=/work/chroot
mount "$PI_ROOT" "$MOUNT_DIR"

cp -r /src/* $MOUNT_DIR/opt/switch-pi-bot

mount -v --bind /dev $MOUNT_DIR/dev
mount -v --bind /dev/pts $MOUNT_DIR/dev/pts
mount -v --bind /proc $MOUNT_DIR/proc
mount -v --bind /sys $MOUNT_DIR/sys

install() {
	# Install Requirements
	apt update
	apt upgrade -y
	curl -fsSL https://deb.nodesource.com/setup_20.x -o nodesource_setup.sh
	bash nodesource_setup.sh
	apt-mark hold linux-image-*
	apt-mark hold linux-headers-*
	apt install -y python3 python3-venv python3-pip tesseract-ocr git wget curl nodejs/nodistro nginx
	rm nodesource_setup.sh
	apt-mark unhold linux-image-*
	apt-mark unhold linux-headers-*

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

	ln -sf /opt/switch-pi-bot/config.txt /boot/firmware/config.txt
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
	ln -sf /etc/nginx/sites-available/pibot /etc/nginx/sites-enabled/pibot
	systemctl restart nginx

	# Enable Services
	systemctl daemon-reload
	systemctl enable usbgadget pibot-backend pibot-frontend
}

chroot $MOUNT_DIR /bin/bash -c "$(declare -f install); install"

exit_chroot() {
	umount $MOUNT_DIR/dev
	umount $MOUNT_DIR/dev/pts
	umount $MOUNT_DIR/proc
	umount $MOUNT_DIR/sys

	USED_SPACE=$(df --block-size=1 "$MOUNT_DIR" | awk 'NR==2 {print $3}')
	MIN_SIZE=$((USED_SPACE + (1024 * 1024 * 100)))
	echo "$MIN_SIZE" >/finalsize/bytes
}

trap exit_chroot EXIT INT
