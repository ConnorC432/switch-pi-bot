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
#include <map>

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

            CROW_ROUTE(app, "/program/start").methods("POST"_method)
            ([this](const crow::request& req){
                auto body = crow::json::load(req.body);
                if (!body || !body.has("programName")) {
                    return crow::response(400, "Missing programName");
                }

                std::string category = body["category"].s();
                std::string programName = body["programName"].s();

                std::map<std::string, crow::json::rvalue> args;

                if (body.has("settings")) {
                    for (auto &item: body["settings"]) {
                        std::string argName = item["argName"].s();
                        // Keep the original value type from JSON
                        args[argName] = item["value"];
                    }
                }

                auto result = worker.startProgram(category, programName, args);
                return crow::response(result);
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
                worker.bindStatusWebSocket(&conn);
            })
            .onclose([this](crow::websocket::connection& conn,
                        const std::string& reason,
                        const uint8_t flags){
                worker.unbindStatusWebSocket();
                std::cout << "Program Status WS disconnected: " << reason << std::endl;
            });

            CROW_ROUTE(app, "/program/logs")
            .websocket(&app)
            .onopen([this](crow::websocket::connection& conn){
                std::cout << "Program WS connected" << std::endl;
                worker.bindOutputWebSocket(&conn);
            })
            .onclose([this](crow::websocket::connection& conn,
                        const std::string& reason,
                        const uint8_t flags){
                worker.unbindOutputWebSocket();
                std::cout << "Program Output WS disconnected: " << reason << std::endl;
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

    ProgramRunner::ProgramRegistry::instance().init();

    ProgramRunner::ProgramRunner programRunner(cap);
    programRunner.run(8080);

    return 0;
}