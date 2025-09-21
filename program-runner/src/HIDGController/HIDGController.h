t//
// Created by connor on 17/09/2025.
//

#ifndef SWITCH_BOT_TEMP_HIDGCONTROLLER_H
#define SWITCH_BOT_TEMP_HIDGCONTROLLER_H

#include <cstdint>
#include <array>
#include <string>
#include <vector>

namespace HIDGController {
    class HIDGController {
    private:
        constexpr DefaultReport = {
            0x0000088080808000
        };

        std::string devicePath;

        using HidReport = std::array<uint8_t, 8>;
        HidReport report;

        void sendReport();
        void clearReport();

        std::mutex reportMutex;

    public:
        explicit HIDGController(std::string devicePath = "/dev/hidg0");

        void resetReport();
    };
} // HIDGController

#endif //SWITCH_BOT_TEMP_HIDGCONTROLLER_H