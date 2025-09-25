//
// Created by connor on 21/09/2025.
//

#include "ImageRecognition.h"
#include <chrono>
#include <thread>
#include <iostream>
#include <opencv4/opencv2/opencv.hpp>
#include <opencv4/opencv2/core.hpp>
#include <opencv4/opencv2/core/types.hpp>
#include <opencv4/opencv2/imgcodecs.hpp>

namespace Capture {

    bool ImageRecognition::findImage(
        const std::string &imagePath,
        int timeoutMs,
        double threshold,
        const std::optional<ROI> &roi
    ) {
        // Set threshold
        double matchThreshold = (threshold > 0) ? threshold : defaultThreshold;

        // Load the target image
        cv::Mat target = cv::imread(imagePath, cv::IMREAD_COLOR);
        if (target.empty()) {
            std::cerr << "Failed to load target image: " << imagePath << std::endl;
            return false;
        }

        auto start = std::chrono::steady_clock::now();

        while (true) {
            cv::Mat frame = capture.grabFrame();
            if (frame.empty()) {
                std::cerr << "Capture returned empty frame\n";
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                continue;
            }

            cv::Rect searchArea(0, 0, frame.cols, frame.rows);
            if (roi.has_value()) {
                const ROI& r = roi.value();
                int w = (r.width > 0) ? r.width : frame.cols - r.x;
                int h = (r.height > 0) ? r.height : frame.rows - r.y;
                searchArea = cv::Rect(r.x, r.y, w, h) & cv::Rect(0, 0, frame.cols, frame.rows);
            }

            cv::Mat searchRegion = frame(searchArea);

            int result_cols = searchRegion.cols - target.cols + 1;
            int result_rows = searchRegion.rows - target.rows + 1;
            if (result_cols <= 0 || result_rows <= 0) {
                return false;
            }

            cv::Mat result(result_rows, result_cols, CV_32FC1);
            cv::matchTemplate(searchRegion, target, result, cv::TM_CCOEFF_NORMED);

            double minVal, maxVal;
            cv::Point minLoc, maxLoc;
            cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);

            if (maxVal >= matchThreshold) {
                cv::Rect matchRect(maxLoc.x + searchArea.x,
                                   maxLoc.y + searchArea.y,
                                   target.cols,
                                   target.rows);
                return true; // Valid image match
            }

            auto now = std::chrono::steady_clock::now();
            if (std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count() > timeoutMs) {
                return false; // timeout
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        return false;
    }

} // Capture
