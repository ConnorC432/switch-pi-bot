//
// Created by connor on 21/09/2025.
//

#ifndef SWITCH_PI_BOT_TEXTRECOGNITION_H
#define SWITCH_PI_BOT_TEXTRECOGNITION_H

#include "Capture.h"
#include <string>
#include <optional>
#include <opencv4/opencv2/opencv.hpp>
#include <tesseract/baseapi.h>

namespace Capture {
    class TextRecognition {
    private:
        Capture& capture;
        tesseract::TessBaseAPI* ocr = nullptr;
        bool initialised = false;

    public:
        explicit TextRecognition(Capture& cap,
                                 const std::string& language = "eng",
                                 const std::string& datapath = "");
        ~TextRecognition();

        bool findText(
            const std::string& searchText,
            int timeoutMs = 1000,
            const std::optional<ROI>& roi = std::nullopt
        );
    };
} // Capture

#endif //SWITCH_PI_BOT_TEXTRECOGNITION_H
