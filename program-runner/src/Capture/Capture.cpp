//
// Created by connor on 21/09/2025.
//

#include "Capture.h"

#include <cstdint>
#include <cstring>
#include <cerrno>
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>
#include <sys/mman.h>
#include <opencv4/opencv2/opencv.hpp>
#include <opencv4/opencv2/core.hpp>
#include <opencv4/opencv2/core/types.hpp>
#include <opencv4/opencv2/imgcodecs.hpp>


namespace Capture {
    Capture::Capture(const std::string& devicePath, uint32_t width, uint32_t height, uint32_t fps)
        : devicePath(devicePath),
        fd(-1),
        width(width),
        height(height),
        fps(fps),
        buffer(nullptr),
        bufferLength(0) {}

    Capture::~Capture() {
        close();
    }

    bool Capture::open() {
        fd = ::open(devicePath.c_str(), O_RDWR);
        if (fd < 0) {
            std::cerr << "Failed to open device: " << devicePath
            << " | " << strerror(errno) << std::endl;
            return false;
        }

        if (!initDevice()) {
            ::close(fd);
            fd = -1;
            return false;
        }

        return true;
    }

    void Capture::close() {
        if (buffer) {
            munmap(buffer, bufferLength);
            buffer = nullptr;
        }
        if (fd >= 0) {
            ::close(fd);
            fd = -1;
        }
    }

    bool Capture::initDevice() {
        // Set format
        v4l2_format fmt {};
        fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        fmt.fmt.pix.width = width;
        fmt.fmt.pix.height = height;
        fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB24;
        fmt.fmt.pix.field = V4L2_FIELD_NONE;

        if (ioctl(fd, VIDIOC_S_FMT, &fmt) < 0) {
            std::cerr << "VIDIOC_S_FMT failed: " << strerror(errno) << std::endl;
            return false;
        }

        // Buffers
        v4l2_requestbuffers req {};
        req.count = 1;
        req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        req.memory = V4L2_MEMORY_MMAP;

        if (ioctl(fd, VIDIOC_REQBUFS, &req) < 0) {
            std::cerr << "VIDIOC_REQBUFS failed: " << strerror(errno) << std::endl;
            return false;
        }

        // Map buffer
        v4l2_buffer buf {};
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = 0;

        if (ioctl(fd, VIDIOC_QUERYBUF, &buf) < 0) {
            std::cerr << "VIDIOC_QUERYBUF failed: " << strerror(errno) << std::endl;
            return false;
        }

        bufferLength = buf.length;
        buffer = static_cast<uint8_t*>(mmap(nullptr, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset));
        if (buffer == MAP_FAILED) {
            std::cerr << "mmap failed: " << strerror(errno) << std::endl;
            return false;
        }

        // Queue buffer
        if (ioctl(fd, VIDIOC_QBUF, &buf) < 0) {
            std::cerr << "VIDIOC_QBUF failed: " << strerror(errno) << std::endl;
            return false;
        }

        // Start Streaming
        int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        if (ioctl(fd, VIDIOC_STREAMON, &type) < 0) {
            std::cerr << "VIDIOC_STREAMON failed: " << strerror(errno) << std::endl;
            return false;
        }

        return true;
    }

    cv::Mat Capture::grabFrame() {
        if (fd < 0) return cv::Mat();

        v4l2_buffer buf {};
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;

        if (ioctl(fd, VIDIOC_DQBUF, &buf) < 0) {
            std::cerr << "VIDIOC_DQBUF failed: " << strerror(errno) << std::endl;
            return cv::Mat();
        }

        // Requeue buffer
        if (ioctl(fd, VIDIOC_QBUF, &buf) < 0) {
            std::cerr << "VIDIOC_QBUF failed: " << strerror(errno) << std::endl;
            return cv::Mat();
        }
        cv::Mat frame(height, width, CV_8UC3, buffer);
        return frame.clone();
    }
} // Capture