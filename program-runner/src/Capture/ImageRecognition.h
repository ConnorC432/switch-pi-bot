//
// Created by connor on 21/09/2025.
//

#ifndef SWITCH_PI_BOT_IMAGERECOGNITION_H
#define SWITCH_PI_BOT_IMAGERECOGNITION_H

#include "Capture.h"
#include <string>
#include <optional>
#include <opencv4/opencv2/opencv.hpp>

namespace Capture {
    class ImageRecognition {
    private:
        Capture& capture;
        double defaultThreshold = 0.8;

    public:
        explicit ImageRecognition(Capture& cap) : capture(cap) {}
        ~ImageRecognition() = default;

        ///
        /// \param imagePath Absolute path to reference image to search for
        /// \param timeoutMs Time in milliseconds to search for image before timing out
        /// \param threshold Image match threshold, recommended value is 0.8
        /// \param roi Optional Bounding Box ROI to limit search area
        /// \return bool - True for match, False for no match
        bool findImage(
            const std::string& imagePath,
            int timeoutMs = 1000,
            double threshold = -1,
            const std::optional<ROI>& roi = std::nullopt
        );
    };
} // Capture

#endif //SWITCH_PI_BOT_IMAGERECOGNITION_H
