//
// Created by connor on 21/09/2025.
//

#include "TextRecognition.h"
#include "Capture.h"
#include <chrono>
#include <thread>
#include <iostream>
#include <algorithm>

namespace Capture {
    TextRecognition::TextRecognition(Capture::Capture& cap, const std::string &language)
        : capture(cap)
    {
        const char* path = nullptr;

        if (ocr.Init(path, language.c_str())) {
            std::cerr << "Tesseract initialisation failed" << std::endl;
            initialised = false;
        } else {
            initialised = true;
        }
    }

    TextRecognition::~TextRecognition() {
        if (initialised) {
            ocr.End();
        }
    }

    bool TextRecognition::findText(const std::string &searchText, int timeoutMs,
                                   const std::optional <Capture::ROI> &roi) {
        if (!initialised) return false;

        auto start = std::chrono::steady_clock::now();

        while (true) {
            cv::Mat frame = capture.getFrame(); // grab latest frame
            if (frame.empty()) {
                std::cerr << "Capture returned empty frame\n";
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                continue;
            }

            cv::Mat roiMat;
            if (roi.has_value()) {
                const ROI& r = roi.value();
                int w = (r.width > 0) ? r.width : frame.cols - r.x;
                int h = (r.height > 0) ? r.height : frame.rows - r.y;
                cv::Rect rect(r.x, r.y, w, h);
                rect &= cv::Rect(0, 0, frame.cols, frame.rows); // clamp
                roiMat = frame(rect).clone();
            } else {
                roiMat = frame.clone();
            }

            cv::Mat gray;
            if (roiMat.channels() == 3) {
                cv::cvtColor(roiMat, gray, cv::COLOR_BGR2GRAY);
            } else {
                gray = roiMat;
            }

            ocr.SetImage(gray.data, gray.cols, gray.rows, gray.channels(), static_cast<int>(gray.step));
            char* outText = ocr.GetUTF8Text();

            bool found = false;
            if (outText) {
                std::string result(outText);
                delete[] outText;

                std::string lowerResult = result;
                std::string lowerSearch = searchText;
                std::transform(lowerResult.begin(), lowerResult.end(), lowerResult.begin(), ::tolower);
                std::transform(lowerSearch.begin(), lowerSearch.end(), lowerSearch.begin(), ::tolower);

                if (lowerResult.find(lowerSearch) != std::string::npos) {
                    found = true;
                }
            }

            if (found) return true;

            auto now = std::chrono::steady_clock::now();
            if (std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count() > timeoutMs) {
                return false; // timed out
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        return false;
    }
} // Capture