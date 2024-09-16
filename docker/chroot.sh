#!/bin/bash

wget -o=pi-bot.img https://downloads.raspberrypi.com/raspios_lite_arm64/images/raspios_lite_arm64-2024-07-04/2024-07-04-raspios-bookworm-arm64-lite.img.xz
xz --decompress -v pi-bot.img

IMG_FILE="pi-bot.img"
MOUNT_DIR="/mnt/chroot"
LOOP_DEVICE=$(losetup -f)

mkdir -p $MOUNT_DIR

losetup "$LOOP_DEVICE" $IMG_FILE
kpartx -av "$LOOP_DEVICE"

ROOT_PARTITION=$(ls /dev/mapper/loop* | grep -E '^/dev/loop[0-9]+p2$')
if [ -z "$ROOT_PARTITION" ]; then
	exit 1
fi

mount "$ROOT_PARTITION" $MOUNT_DIR

mount --bind /dev $MOUNT_DIR/dev
mount --bind /dev/pts $MOUNT_DIR/dev/pts
mount --bind /proc $MOUNT_DIR/proc
mount --bind /sys $MOUNT_DIR/sys
mount --bind /src $MOUNT_DIR/src

chroot $MOUNT_DIR /bin/bash -c "bash /usr/local/bin/install.sh"

exit_chroot() {
	umount $MOUNT_DIR/dev
	umount $MOUNT_DIR/dev/pts
	umount $MOUNT_DIR/proc
	umount $MOUNT_DIR/sys
	umount $MOUNT_DIR/src
	losetup -d $LOOP_DEVICE

	cp $IMG_FILE /build/$IMG_FILE
}

trap exit_chroot EXIT INT
