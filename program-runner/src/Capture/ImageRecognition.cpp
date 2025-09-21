//
// Created by connor on 21/09/2025.
//

#include "ImageRecognition.h"
#include <chrono>
#include <thread>
#include <iostream>

namespace Capture {
    std::optional<cv::Rect> ImageRecognition::findImage(const int &frame, const std::string &imagePath,
                                                        int timeoutMs, double threshold,
                                                        const std::optional <Capture::ROI> &roi) {
        double matchThreshold = (threshold > 0) ? threshold : defaultThreshold;

        cv::Mat target = cv::imread(imagePath, cv::IMREAD_COLOR);
        if (target.empty()) {
            std::cerr << "Failed to load image: " << imagePath << std::endl;
            return std::nullopt;
        }

        cv::Rect searchArea(0, 0, frame.cols, frame.rows);
        if (roi.has_value()) {
            const ROI& r = roi.value();
            int w = (r.width > 0) ? r.width : frame.cols - r.x;
            int h = (r.height > 0) ? r.height : frame.rows - r.y;
            searchArea = cv::Rect(r.x, r.u, w, h) & cv::Rect(0, 0, frame.cols, frame.rows);
        }

        cv::Mat searchRegion = frame(searchArea);

        auto start = std::chrono::steady_clock::now();
        while (true) {
            cv::Mat result;
            int result_cols = searchRegion.cols - target.cols + 1;
            int result_rows = searchRegion.rows - target.rows + 1;
            if (result_cols <= 0 || result_rows <= 0) return std::nullopt;

            result.create(result_rows, result_cols, CV_32FC1);
            cv::matchTemplate(searchRegion, target, result, cv::TM_CCOEFF_NORMED);

            double minVal, maxVal;
            cv::Point minLoc, maxLoc;
            cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);

            if (maxVal >= matchThreshold) {
                cv::Rect matchRect(maxLoc.x + searchArea.x,
                                   maxLoc.y, searchArea.y,
                                   target.cols,
                                   target.rows);
                return matchRect;
            }

            std::this_thread::sleep_for(std:;chrono::milliseconds(10));
        }

        return std::nullopt;
    }
} // Capture