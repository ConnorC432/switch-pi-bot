//
// Created by connor on 21/09/2025.
//

#include "Capture/Capture.h"
#include "ProgramWorker.h"
#include "ProgramRegistry.h"
#include "Database/Database.h"
#include "Database/Programs.h"
#include <crow.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <vector>
#include <mutex>
#include <iostream>
#include <map>
#include <thread>
#include <chrono>
#include <nlohmann/json.hpp>
#include <cstdlib>

namespace ProgramRunner {
    class ProgramRunner {
    private:
        crow::SimpleApp app;
        Capture::Capture& capture;
        std::vector<crow::websocket::connection*> clients;
        std::mutex frameMutex;
        std::mutex clientsMutex;

        ProgramWorker worker;

        bool cameraOpened = false;

        // Database


        Database::Database db;
        Database::Programs programsDB;

    public:
        explicit ProgramRunner(Capture::Capture& cap)
            : capture(cap),
            db(
                std::getenv("MONGO_HOST") ? std::getenv("MONGO_HOST") : "mongodb://mongodb:27017",
                std::getenv("MONGO_USER") ? std::getenv("MONGO_USER") : "admin",
                std::getenv("MONGO_PASS") ? std::getenv("MONGO_PASS") : "root",
                std::getenv("MONGO_DB") ? std::getenv("MONGO_DB") : "switchPiBot",
                "admin"
            ),
            programsDB(db, "programs")
        {
            cameraOpened = capture.open();
            if (!cameraOpened) {
                std::cerr << "Capture Card failed to open" << std::endl;
            }
        }

        void startStream() {
            std::thread([this]() {
                while (cameraOpened) {
                    cv::Mat frame;
                    {
                        std::lock_guard<std::mutex> lock(frameMutex);
                        frame = capture.grabFrame();
                    }

                    if (!frame.empty()) {
                        std::vector<uchar> buffer;
                        cv::imencode(".jpg", frame, buffer);

                        std::string binaryMessage(reinterpret_cast<char*>(buffer.data()), buffer.size());

                        std::lock_guard<std::mutex> lock(clientsMutex);
                        for (auto* client : clients) {
                            client->send_binary(binaryMessage);
                        }
                    }

                    std::this_thread::sleep_for(std::chrono::milliseconds(16)); // ~60 FPS
                }
            }).detach();
        }

        void setupRoutes() {
            // Capture Card Stream
            CROW_ROUTE(app, "/stream")
            .websocket(&app)
            .onopen([this](crow::websocket::connection& conn){
                std::cout << "WebSocket connected" << std::endl;
                std::lock_guard<std::mutex> lock(clientsMutex);
                clients.push_back(&conn);
            })

            .onclose([this](crow::websocket::connection& conn,
                    const std::string& reason,
                    const uint8_t flags){
                std::cout << "WebSocket disconnected: " << reason << std::endl;
                std::lock_guard<std::mutex> lock(clientsMutex);
                clients.erase(std::remove(clients.begin(), clients.end(), &conn), clients.end());
            });

            // Programs
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

            // Database
            CROW_ROUTE(app, "/database/programs")
            ([this]() {
                auto programs = programsDB.getPrograms();
                nlohmann::json result = nlohmann::json::array();

                for (const auto& p : programs) {
                    nlohmann::json prog_json;
                    prog_json["programName"] = p.programName;
                    prog_json["displayName"] = p.displayName;
                    prog_json["description"] = p.description;
                    prog_json["category"] = p.category;

                    prog_json["settings"] = nlohmann::json::array();
                    for (const auto& s : p.settings) {
                        nlohmann::json s_json;
                        s_json["argName"] = s.argName;
                        s_json["displayName"] = s.displayName;

                        std::visit([&s_json](auto&& v){
                            s_json["value"] = v;
                        }, s.value);

                        prog_json["settings"].push_back(s_json);
                    }

                    result.push_back(prog_json);
                }

                return crow::response(result.dump());
            });

            CROW_ROUTE(app, "/database/programs/update").methods(crow::HTTPMethod::Post)
            ([this](const crow::request& req){
                auto body = nlohmann::json::parse(req.body, nullptr, false);
                if (body.is_discarded()) return crow::response(400, "Invalid JSON");

                std::string programName = body.value("programName", "");
                std::string category = body.value("category", "");

                auto setting_json = body["setting"];
                if (!setting_json.is_object()) return crow::response(400, "Missing setting object");

                Database::Setting s;
                s.argName = setting_json.value("argName", "");
                s.displayName = setting_json.value("displayName", "");

                if (setting_json.contains("value")) {
                    if (setting_json["value"].is_number_integer())
                        s.value = setting_json["value"].get<int>();
                    else if (setting_json["value"].is_number_float())
                        s.value = setting_json["value"].get<double>();
                    else if (setting_json["value"].is_boolean())
                        s.value = setting_json["value"].get<bool>();
                    else if (setting_json["value"].is_string())
                        s.value = setting_json["value"].get<std::string>();
                }

                bool ok = programsDB.updatePrograms(programName, category, s);
                if (ok)
                    return crow::response(200, "Updated successfully");
                else
                    return crow::response(500, "Update failed");
            });
        }

        void run(int port = 8080) {
            setupRoutes();
            startStream();
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