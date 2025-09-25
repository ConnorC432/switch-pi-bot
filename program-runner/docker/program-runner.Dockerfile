FROM ubuntu:22.04
LABEL authors="ConnorC432"

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
        locales \
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
        libmongoc-dev \
        libbson-dev \
        libboost-all-dev \
        nlohmann-json3-dev \
        ca-certificates \
    && apt clean \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /mongo

RUN curl -OL https://github.com/mongodb/mongo-c-driver/releases/download/2.1.0/mongo-c-driver-2.1.0.tar.gz \
    && tar -xzf mongo-c-driver-2.1.0.tar.gz

RUN mkdir -p mongo-c-driver-2.1.0/build && cd mongo-c-driver-2.1.0/build \
    && cmake .. -DCMAKE_BUILD_TYPE=Release \
      -DENABLE_AUTOMATIC_INIT_AND_CLEANUP=OFF \
      -DCMAKE_INSTALL_PREFIX=/usr \
    && cmake --build . -- -j$(nproc) \
    && cmake --install .

RUN curl -OL https://github.com/mongodb/mongo-cxx-driver/releases/download/r4.1.2/mongo-cxx-driver-r4.1.2.tar.gz \
    && tar -xzf mongo-cxx-driver-r4.1.2.tar.gz

RUN mkdir -p mongo-cxx-driver-r4.1.2/build && cd mongo-cxx-driver-r4.1.2/build \
    && cmake .. -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_CXX_STANDARD=17 \
        -DCMAKE_INSTALL_PREFIX=/usr \
    && cmake --build . -- -j$(nproc) \
    && cmake --install .

WORKDIR /app

COPY ./include/crow /app/include/crow
COPY ./CMakeLists.txt /app/
COPY ./src /app/src/

RUN mkdir -p build && cd build && cmake .. && cmake --build . -- -j$(nproc)

EXPOSE 8080

CMD ["./build/ProgramRunner"]