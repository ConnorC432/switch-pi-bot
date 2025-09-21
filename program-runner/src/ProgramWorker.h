//
// Created by connor on 21/09/2025.
//

#ifndef PROGRAMRUNNER_PROGRAMWORKER_H
#define PROGRAMRUNNER_PROGRAMWORKER_H

#pragma once

#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <crow.h>
#include <crow/websocket.h>

namespace ProgramRunner {

    class ProgramWorker {
    private:
        std::atomic<pid_t> runningPid{-1};
        std::string currentProgram;
        std::thread captureThread;
        std::mutex programLock;

        crow::websocket::connection* statusWs{nullptr};
        std::mutex statusWsLock;

        crow::websocket::connection* outputWs{nullptr};
        std::mutex outputWsLock;

        void sendWebSocket(const std::string& message);
        std::string escapeJson(const std::string &str) const;

    public:
        ProgramWorker() = default;
        ~ProgramWorker();

        void bindStatusWebSocket(crow::websocket::connection* conn);
        void unbindStatusWebSocket();

        void bindOutputWebSocket(crow::websocket::connection* conn);
        void unbindOutputWebSocket();

        void sendStatus(const std::string& message);
        void sendOutput(const std::string& message);

        crow::json::wvalue startProgram(const std::string& program, const std::vector<std::string>& args);

        crow::json::wvalue killProgram();
    };

} // ProgramRunner

#endif //PROGRAMRUNNER_PROGRAMWORKER_H
