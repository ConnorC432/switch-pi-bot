//
// Created by connor on 21/09/2025.
//

#ifndef SWITCH_PI_BOT_TEXTRECOGNITION_H
#define SWITCH_PI_BOT_TEXTRECOGNITION_H

#include <string>
#include <optional>
#include <opencv2/opencv.hpp>
#include <tesseract/baseapi.h>

namespace Capture {
    class TextRecognition {
    private:
        Capture::Capture& capture;
        tesseract::TessBaseAPI ocr;
        bool initialised = false;

    public:
        TextRecognition(Capture::Capture& cap, const std::string& language = "eng", const std::string& datapath = "");
        ~TextRecognition();

        bool finxText(
            const cv::Mat& frame,
            const std::string& searchText,
            int timeoutMs = 1000,
            const std::optional<ROI>& roi = std::nullopt
        );
    };
} // Capture

#endif //SWITCH_PI_BOT_TEXTRECOGNITION_H
