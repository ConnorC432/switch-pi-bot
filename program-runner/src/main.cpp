//
// Created by connor on 21/09/2025.
//

#include "Capture/Capture.h"
#include "ProgramWorker.h"
#include "ProgramRegistry.h"
#include <crow.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <vector>
#include <mutex>
#include <iostream>

namespace ProgramRunner {
    class ProgramRunner {
    private:
        crow::SimpleApp app;
        Capture::Capture& capture;
        std::mutex frameMutex;

        ProgramWorker worker;

        bool cameraOpened = false;

    public:
        explicit ProgramRunner(Capture::Capture& cap) : capture(cap) {
            cameraOpened = capture.open();
            if (!cameraOpened) {
                std::cerr << "Capture Card failed to open" << std::endl;
            }
        }

        void setupRoutes() {
            CROW_ROUTE(app, "/stream")
            .websocket(&app)
            .onopen([](crow::websocket::connection& conn){
                std::cout << "WebSocket connected" << std::endl;
            })

            .onclose([](crow::websocket::connection& conn,
                    const std::string& reason,
                    const uint8_t flags){
                std::cout << "WebSocket disconnected: " << reason << std::endl;
            })

            .onmessage([this](crow::websocket::connection& conn,
                    const std::string&, bool){
                cv::Mat frame;
                {
                    std::lock_guard<std::mutex> lock(frameMutex);
                    frame = capture.grabFrame();
                }

                if (!frame.empty()) {
                    std::vector<uchar> buffer;
                    cv::imencode(".jpg", frame, buffer);
                    std::string binaryMessage(buffer.begin(), buffer.end());
                    conn.send_binary(binaryMessage);
                }
            });

            CROW_ROUTE(app, "/program/start")
            .methods(crow::HTTPMethod::POST)
                ([this](const crow::request& req){
                   auto qs = crow::query_string(req.url);
                   auto name = qs.get("program");
                   if (!name) {
                       return crow::response(400, "Missing Program name");
                   }

                   auto func = ProgramRegistry::instance().getProgram(name);
                   if (!func) {
                       return crow::response(404, "Program not found");
                   }

                   return crow::response(worker.startProgram(name, {}));
                });

            CROW_ROUTE(app, "/program/kill")
            .methods(crow::HTTPMethod::POST)
            ([this](const crow::request&){
                return crow::response(worker.killProgram());
            });

            CROW_ROUTE(app, "/program/status")
            .websocket(&app)
            .onopen([this](crow::websocket::connection& conn){
                std::cout << "Program WS connected" << std::endl;
                worker.bindWebSocket(&conn);
                worker.status();
            })
            .onclose([this](crow::websocket::connection& conn,
                        const std::string& reason,
                        const uint8_t flags){
                std::cout << "Program WS disconnected: " << reason << std::endl;
                worker.bindWebSocket(nullptr);
            });

            CROW_ROUTE(app, "/status")
            ([](){
                crow::json::wvalue x;
                x["status"] = "ok";
                return crow::response(x);
            });
        }

        void run(int port = 8080) {
            setupRoutes();
            std::cout << "Starting server..." << std::endl;
            app.bindaddr("0.0.0.0").port(port).multithreaded().run();
        }
    };
}

int main() {
    Capture::Capture cap("/dev/video0", 1920, 1080, 30);

    ProgramRunner::ProgramRunner programRunner(cap);
    programRunner.run(8080);

    return 0;
}