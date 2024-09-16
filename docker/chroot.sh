#!/bin/bash

if [ -z "$PI_ROOT" ]; then
	exit 1
fi

mkdir -vp /mnt/chroot
MOUNT_DIR=/mnt/chroot
mount "$PI_ROOT" "$MOUNT_DIR"

mount -v --bind /dev $MOUNT_DIR/dev
mount -v --bind /dev/pts $MOUNT_DIR/dev/pts
mount -v --bind /proc $MOUNT_DIR/proc
mount -v --bind /sys $MOUNT_DIR/sys
mkdir -vp $MOUNT_DIR/src
mount -v --bind /src $MOUNT_DIR/src

chroot $MOUNT_DIR /bin/bash -c "bash /usr/local/bin/install.sh"

exit_chroot() {
	umount $MOUNT_DIR/dev
	umount $MOUNT_DIR/dev/pts
	umount $MOUNT_DIR/proc
	umount $MOUNT_DIR/sys
	umount $MOUNT_DIR/src
	losetup -vd "$LOOP_DEVICE"
}

trap exit_chroot EXIT INT
