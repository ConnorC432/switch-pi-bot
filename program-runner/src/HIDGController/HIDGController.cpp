//
// Created by connor on 17/09/2025.
//

#include "HIDGController.h"
#include <fstream>
#include <iostream>
#include <mutex>
#include <stdexcept>

namespace HIDGController {
    HIDGController::HIDGController(std::string devicePath)
        : devicePath(devicePath), report(DefaultReport) {}

    void HIDGController::init() {
        clearReport();
    }

    void HIDGController::sendReport() {
        std::lock_guard<std::mutex> lock(reportMutex);

        std::ofstream device(devicePath, std::ios::binary);
        if (!device) {
            std::cerr << "Failed to open device: " << devicePath << std::endl;
            return;
        }
        device.write(reinterpret_cast<const char*>(report.data()), report.size());
        if (!device) {
            std::cerr << "Failed to write report to device: " << devicePath << std::endl;
        }
    }

    void HIDGController::clearReport() {
        std::lock_guard<std::mutex> lock(reportMutex);

        report = DefaultReport;
        sendReport();
    }
} // HIDGController