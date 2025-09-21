//
// Created by connor on 21/09/2025.
//

#ifndef SWITCH_PI_BOT_CAPTURE_H
#define SWITCH_PI_BOT_CAPTURE_H

#include <cstdint>
#include <string>

namespace Capture {
    class Capture {
    private:
        std::string devicePath = 0;
        int fd;
        uint32_t width = 1920;
        uint32_t height = 1080;
        uint32_t fps = 30;
        uint8_t buffer;
        size_t bufferLength;

    public:
        Capture(const std::string& devicePath = "/dev/video0",
                uint32_t width, uint32_t height, uint32_t fps);
        ~Capture();

        bool open();
        void close();

        uint8_t* grabFrame();

        uint32_t getWidth() const { return width; }
        uint32_t getHeight() const { return height; }
        uint32_t getFPS() const { return fps; }
    };
} // Capture

#endif //SWITCH_PI_BOT_CAPTURE_H