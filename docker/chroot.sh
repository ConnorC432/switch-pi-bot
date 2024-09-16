#!/bin/bash

wget -v -O pi-bot.img.xz https://downloads.raspberrypi.com/raspios_lite_arm64/images/raspios_lite_arm64-2024-07-04/2024-07-04-raspios-bookworm-arm64-lite.img.xz
xz --decompress -v pi-bot.img.xz

IMG_FILE="pi-bot.img"
MOUNT_DIR="/mnt/chroot"
LOOP_DEVICE=$(losetup -f)

mkdir -vp $MOUNT_DIR

losetup -v "$LOOP_DEVICE" $IMG_FILE
kpartx -av "$LOOP_DEVICE"

ROOT_PARTITION=$(ls /dev/mapper/loop* | grep -E '^/dev/loop[0-9]+p2$')
if [ -z "$ROOT_PARTITION" ]; then
	exit 1
fi

mount "$ROOT_PARTITION" $MOUNT_DIR

mount -v --bind /dev $MOUNT_DIR/dev
mount -v --bind /dev/pts $MOUNT_DIR/dev/pts
mount -v --bind /proc $MOUNT_DIR/proc
mount -v --bind /sys $MOUNT_DIR/sys
mount -v --bind /src $MOUNT_DIR/src

chroot $MOUNT_DIR /bin/bash -c "bash /usr/local/bin/install.sh"

exit_chroot() {
	umount $MOUNT_DIR/dev
	umount $MOUNT_DIR/dev/pts
	umount $MOUNT_DIR/proc
	umount $MOUNT_DIR/sys
	umount $MOUNT_DIR/src
	losetup -vd "$LOOP_DEVICE"

	cp -v $IMG_FILE /build/$IMG_FILE
}

trap exit_chroot EXIT INT
