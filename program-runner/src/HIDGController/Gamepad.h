//
// Created by connor on 21/09/2025.
//

#ifndef SWITCH_PI_BOT_GAMEPAD_H
#define SWITCH_PI_BOT_GAMEPAD_H

#include "HIDGController.h"
#include <vector>
#include <string>
#include <array>
#include <mutex>

namespace HIDGController {
    class Gamepad {
    private:
        HIDGController& controller;

        std::mutex buttonMutex;
        std::mutex dpadMutex;
        std::mutex leftStickMutex;
        std::mutex rightStickMutex;

    public:
        static constexpr std::array<uint8_t, 8> DefaultReport = {0x00,0x00,0x08,0x80,0x80,0x80,0x80,0x00};

        // Buttons
        enum class Button : uint16_t {
            Y       = 0x0001,
            B       = 0x0002,
            A       = 0x0004,
            X       = 0x0008,
            L       = 0x0010,
            R       = 0x0020,
            ZL      = 0x0040,
            ZR      = 0x0080,
            Minus   = 0x0100,
            Plus    = 0x0200,
            L3      = 0x0400,
            R3      = 0x0800,
            Power   = 0x1000,
            Capture = 0x2000
        };

        // DPad
        enum class DPad : uint8_t {
            Center = 0x08,
            N      = 0x00,
            NE     = 0x01,
            E      = 0x02,
            SE     = 0x03,
            S      = 0x04,
            SW     = 0x05,
            W      = 0x06,
            NW     = 0x07
        };

        // Sticks
        enum class Stick : uint16_t {
            Center = 0x8080,
            N      = 0x8000,
            NE     = 0xFF00,
            E      = 0xFF80,
            SE     = 0xFFFF,
            S      = 0x80FF,
            SW     = 0x00FF,
            W      = 0x0080,
            NW     = 0x0000
        };

        explicit Gamepad(HIDGController& controller);

        void pressButton(const std::vector<Button>& buttons, int holdMs = 100);
        void moveDPad(DPad direction, int holdMs = 100);
        void moveLeftStick(Stick direction, int holdMs = 100);
        void moveRightStick(Stick direction, int holdMs = 100);
    };
} // HIDGController

#endif //SWITCH_PI_BOT_GAMEPAD_H