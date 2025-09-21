//
// Created by connor on 21/09/2025.
//

#include "Gamepad.h"
#include "HIDGController.h"
#include <thread>
#include <chrono>
#include <iostream>

namespace HIDGController {
    Gamepad::Gamepad(HIDGController& controller)
        : controller(controller) {}

    static inline uint8_t stickX(Gamepad::Stick s) { return static_cast<uint16_t>(s) >> 8; }
    static inline uint8_t stickY(Gamepad::Stick s) { return static_cast<uint16_t>(s) & 0xFF; }

    // Buttons
    void Gamepad::pressButton(const std::vector<Button>& buttons, int holdMs) {
        std::lock_guard<std::mutex> lock(buttonMutex);

        uint16_t added = 0;
        for (auto button : buttons) added |= static_cast<uint16_t>(button);

        controller.setReportWord(0, added);

        std::thread([this, added, holdMs]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(holdMs));
            controller.clearReport();
        }).detach();
    }

    // DPad
    void Gamepad::moveDPad(DPad direction, int holdMs) {
        std::lock_guard<std::mutex> lock(dpadMutex);

        controller.setReportByte(2, static_cast<uint8_t>(direction));

        std::thread([this, holdMs]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(holdMs));
            controller.clearReport();
        }).detach();
    }

    // Left Stick
    void Gamepad::moveLeftStick(Stick direction, int holdMs) {
        std::lock_guard<std::mutex> lock(leftStickMutex);

        uint8_t x = stickX(direction);
        uint8_t y = stickY(direction);

        controller.setReportByte(3, x);
        controller.setReportByte(4, y);

        std::thread([this, holdMs]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(holdMs));
            controller.clearReport();
        }).detach();
    }

    // Right Stick
    void Gamepad::moveRightStick(Stick direction, int holdMs) {
        std::lock_guard<std::mutex> lock(rightStickMutex);

        uint8_t x = stickX(direction);
        uint8_t y = stickY(direction);

        controller.setReportByte(5, x);
        controller.setReportByte(6, y);

        std::thread([this, holdMs]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(holdMs));
            controller.clearReport();
        }).detach();
    }
} // HIDGController