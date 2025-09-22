FROM ubuntu:22.04
LABEL authors="connor"

ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=Etc/UTC

RUN apt update && apt upgrade -y

RUN apt update \
    && apt install -y --no-install-recommends \
        build-essential \
        gcc \
        g++ \
        gdb \
        clang \
        make \
        ninja-build \
        cmake \
        autoconf \
        automake \
        libtool \
        valgrind \
        locales-all \
        dos2unix \
        rsync \
        tar \
        libopencv-dev \
        tesseract-ocr \
        libtesseract-dev \
        libleptonica-dev \
        tesseract-ocr-eng \
        libasio-dev \
        curl \
    && apt clean

WORKDIR /app

COPY ../CMakeLists.txt /app/
COPY ../src /app/src/

RUN mkdir -p build && cd build && cmake .. && cmake --build . -- -j$(nproc)

EXPOSE 8080

CMD ["./build/ProgramRunner"]