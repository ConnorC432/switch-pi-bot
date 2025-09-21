//
// Created by connor on 21/09/2025.
//

#include "Gamepad.h"
#include <thread>
#include <chrono>
#include <iostream>

namespace HIDGController {
    Gamepad::Gamepad(HIDGController& controller)
        : controller(controller) {}

    static uint8_t stickX(HIDGController::Stick s) { return static_cast<uint16_t>(s) >> 8; }
    static uint8_t stickY(HIDGController::Stick s) { return static_cast<uint16_t>(s) & 0xFF; }

    // Input Functions
    void Gamepad::pressButton(const std::vector<Button>& buttons, int holdMs) {
        std::lock_guard<std::mutex> lock(buttonMutex);

        uint16_t added = 0;
        for (auto button : buttons) added |= static_cast<uint16_t>(button);

        {
            std::lock_guard<std::mutex> reportLock(controller.reportMutex);
            uint16_t current = controller.report[0] | (controller.report[1] << 8);
            current |= added;
            controller.report[0] = current & 0xFF;
            controller.report[1] = (current >> 8) & 0xFF;
            controller.sendReport();
        }

        std::thread([this, added, holdMs]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(holdMs));
            std::lock_guard<std::mutex> reportLock(controller.reportMutex);
            uint16_t current = controller.report[0] | (controller.report[1] << 8);
            current &= ~added;
            controller.report[0] = current & 0xFF;
            controller.report[1] = (current >> 8) & 0xFF;
            controller.sendReport();
        }).detach();
    }

    void Gamepad::moveDPad(DPad direction, int holdMs) {
        std::lock_guard<std::mutex> lock(dpadMutex);

        {
            std::lock_guard<std::mutex> reportLock(controller.reportMutex);
            controller.report[2] = static_cast<uint8_t>(direction);
            controller.sendReport();

            std::thread([this, holdMs]() {
                std::this_thread::sleep_for(std::chrono::milliseconds(holdMs));
                std::lock_guard<std::mutex> reportLock(controller.reportMutex);
                controller.report[2] = DefaultReport[2];
                controller.sendReport();
            }).detach();
        }
    }

    void Gamepad::moveLeftStick(Stick direction, int holdMs) {
        std::lock_guard<std::mutex> lock(leftStickMutex);

        uint8_t x = stickX(direction);
        uint8_t y = stickY(direction);

        std::thread([this, x, y, holdMs]() {
            {
                std::lock_guard<std::mutex> reportLock(controller.reportMutex);
                controller.report[3] = x;
                controller.report[4] = y;
                controller.sendReport();
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(holdMs));

            {
                std::lock_guard<std::mutex> reportLock(controller.reportMutex);
                controller.report[3] = DefaultReport[3];
                controller.report[4] = DefaultReport[4];
                controller.sendReport();
            }
        }).detach();
    }

    void Gamepad::moveRightStick(Stick direction, int holdMs) {
        std::lock_guard<std::mutex> lock(rightStickMutex);

        uint8_t x = stickX(direction);
        uint8_t y = stickY(direction);

        std::thread([this, x, y, holdMs]() {
            {
                std::lock_guard<std::mutex> reportLock(controller.reportMutex);
                controller.report[5] = x;
                controller.report[6] = y;
                controller.sendReport();
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(holdMs));

            {
                std::lock_guard<std::mutex> reportLock(controller.reportMutex);
                controller.report[5] = DefaultReport[5];
                controller.report[6] = DefaultReport[6];
                controller.sendReport();
            }
        }).detach();
    }
} // HIDGController