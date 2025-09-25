//
// Created by connor on 17/09/2025.
//

#ifndef SWITCH_BOT_TEMP_HIDGCONTROLLER_H
#define SWITCH_BOT_TEMP_HIDGCONTROLLER_H

#include <cstdint>
#include <array>
#include <string>
#include <vector>
#include <mutex>

namespace HIDGController {
    class HIDGController {
    private:
        static constexpr std::array<uint8_t, 8> DefaultReport = {
            0x00, 0x00, 0x08, 0x80, 0x80, 0x80, 0x80, 0x00
        };

        std::string devicePath;
        std::array<uint8_t, 8> report;
        std::mutex reportMutex;

    public:
        ///
        /// \param devicePath Absolute path to USB HIDG device
        explicit HIDGController(std::string devicePath = "/dev/hidg0");

        void sendReport();
        void clearReport();
        void init();

        void setReportByte(size_t index, uint8_t value) {
            std::lock_guard<std::mutex> lock(reportMutex);
            if (index < report.size()) {
                report[index] = value;
                sendReport();
            }
        }

        void setReportWord(size_t lowIndex, uint16_t value) {
            std::lock_guard<std::mutex> lock(reportMutex);
            if (lowIndex + 1 < report.size()) {
                report[lowIndex] = value & 0xFF;
                report[lowIndex + 1] = (value >> 8) & 0xFF;
                sendReport();
    }
}

    };
} // HIDGController

#endif //SWITCH_BOT_TEMP_HIDGCONTROLLER_H