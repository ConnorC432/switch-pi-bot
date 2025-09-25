//
// Created by connor on 21/09/2025.
//

#ifndef SWITCH_PI_BOT_CAPTURE_H
#define SWITCH_PI_BOT_CAPTURE_H

#pragma once
#include <cstdint>
#include <string>
#include <opencv4/opencv2/opencv.hpp>
#include <opencv4/opencv2/core.hpp>
#include <opencv4/opencv2/core/types.hpp>
#include <opencv4/opencv2/imgcodecs.hpp>

namespace Capture {
    struct ROI {
        int x = 0; /// ROI Top Left X
        int y = 0; /// ROI Top Left Y
        int width = -1; /// ROI Width
        int height = -1; /// ROI Height
    };

    class Capture {
    private:
        std::string devicePath;
        int fd;
        uint32_t width = 1920;
        uint32_t height = 1080;
        uint32_t fps = 30;
        uint8_t* buffer;
        size_t bufferLength;

        std::mutex frameMutex;

        bool initDevice();

    public:
        ///
        /// \param devicePath Absolute path to V4L2 device.
        /// \param width Resolution width
        /// \param height Resolution height
        /// \param fps Capture card FPS
        Capture(const std::string& devicePath = "/dev/video0",
                uint32_t width = 1920,
                uint32_t height = 1080,
                uint32_t fps = 30);

        ~Capture();

        bool open();
        void close();

        cv::Mat grabFrame();

        uint32_t getWidth() const { return width; }
        uint32_t getHeight() const { return height; }
        uint32_t getFPS() const { return fps; }
    };
} // Capture

#endif //SWITCH_PI_BOT_CAPTURE_H