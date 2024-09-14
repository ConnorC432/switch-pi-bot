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
apt purge nodejs npm nodejs-legacy libnode72
apt autoremove
curl -fsSL https://deb.nodesource.com/setup_20.x -o nodesource_setup.sh
bash nodesource_setup.sh
apt-get install -fy nodejs/nodistro
rm nodesource_setup.sh

umount ${ROOTFS_DIR}/proc
umount ${ROOTFS_DIR}/dev
umount ${ROOTFS_DIR}/sys
umount ${ROOTFS_DIR}/run
umount ${ROOTFS_DIR}/tmp