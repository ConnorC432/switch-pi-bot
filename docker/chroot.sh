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

chroot $MOUNT_DIR /bin/bash -c "bash /usr/local/bin/install.sh"

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
