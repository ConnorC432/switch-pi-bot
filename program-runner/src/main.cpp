//
// Created by connor on 21/09/2025.
//

#include "Capture/Capture.h"
#include <crow.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <vector>
#include <mutex>
#include <iostream>

namespace ProgramRunner {
    class ProgramRunner {
    private:
        Capture::Capture& capture;
        std::mutex frameMutex;

    public:
        explicit ProgramRunner(Capture::Capture& cap) : capture(cap) {}

        void setupRoutes() {
            CROW_ROUTE(app, "/stream")
            .websocket()
            .onopen([](crow::websocket::connection& conn){
                std::cout << "WebSocket connected: " << conn.get_id() << std::endl;
            })
            .onclose([](crow::websocket::connection& conn, const std::string& reason){
                std::cout << "WebSocket disconnected: " << conn.get_id()
                          << " Reason : " << reason << std::endl;
            })
            .onmessage([this](crow::websocket::connection& conn,
                    const std::string&, bool){
                cv::Mat frame;
                {
                    std::lock_guard<std::mutex> lock(frameMutex);
                    frame = capture.getFrame();
                }

                if (!frame.empty()) {
                    std::vector<uchar> buffer;
                    cv::imencode(".jpg", frame, buffer);
                    conn.send_binary(buffer);
                }
            });

            CROW_ROUTE(app, "/status")
            ([]{
                crow::json::wvalue x;
                x["status"] = "ok";
                return x;
            });
        }

        void run(int port = 8080) {
            setupRoutes();
            app.port(port).multithreaded().run();
        }
    };
}

int main() {
    Capture::Capture cap("/dev/video0", 1920, 1080, 30);

    ProgramRunner::ProgramRunner programRunner(cap);
    programRunner.run(8080);

    return 0;
}