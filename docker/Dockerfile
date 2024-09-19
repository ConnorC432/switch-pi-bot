#Bind mount source code at /src
#Bind mount finished img file directory at /build

FROM arm32v7/debian:bullseye-slim

# Install Requirements
RUN apt update && apt install -y \
    mount \
    sudo \
    bash \
    e2fsprogs \
    wget \
    util-linux \
    gawk

ADD chroot.sh /usr/local/bin/chroot.sh
RUN chmod +x /usr/local/bin/chroot.sh

WORKDIR /work

CMD ["/usr/local/bin/chroot.sh"]