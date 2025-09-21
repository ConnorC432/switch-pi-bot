//
// Created by connor on 17/09/2025.
//

#include "HIDGController.h"
#include <fstream>
#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>

namespace HIDGController {
    HIDGController::HIDGController(const std::string& devicePath)
        : devicePath(devicePath), report(DefaultReport) {}

    void HIDGController::init() {
        HIDGController::resetReport();
    }

    void HIDGController::sendReport() {
        std::lock_guard<std::mutex> reportLock(reportMutex);
        std::ofstream device(devicePath, std::ios::binary);
        if (!device) {
            std::cerr << "Failed to open device: " << devicePath << std::endl;
            return;
        }
        device.write(reinterpret_cast<const char*>(report.data()), report.size());
    }

    void HIDGController::clearReport() {
        std::lock_guard<std::mutex> reportLock(reportMutex);
        report = DefaultReport;
        sendReport();
    }

    void HIDGController::resetReport() {
        clearReport();
    }
} // HIDGController