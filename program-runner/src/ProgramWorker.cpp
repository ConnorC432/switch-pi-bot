//
// Created by connor on 21/09/2025.
//

#include "ProgramWorker.h"
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <iostream>

namespace ProgramRunner {
    ProgramWorker::~ProgramWorker() {
        killProgram();
        if (captureThread.joinable()) {
            captureThread.join();
        }
    }

    std::string ProgramWorker::escapeJson(const std::string &str) const {
        std::string escaped;
        for (char c : str) {
            switch (c) {
                case '"':  escaped += "\\\""; break;
                case '\\': escaped += "\\\\"; break;
                case '\b': escaped += "\\b";  break;
                case '\f': escaped += "\\f";  break;
                case '\n': escaped += "\\n";  break;
                case '\r': escaped += "\\r";  break;
                case '\t': escaped += "\\t";  break;
                default:   escaped += c;      break;
            }
        }
        return escaped;
    }

    void ProgramWorker::sendWebSocket(const std::string &message) {
        std::lock_guard<std::mutex> lock(wsLock);
        if (wsConnection) {
            try {
                wsConnection->send_text(message);
            } catch (...) {
                wsConnection = nullptr;
            }
        }
    }

    void ProgramWorker::bindWebSocket(crow::websocket::connection *conn) {
        std::lock_guard<std::mutex> lock(wsLock);
        wsConnection = conn;
    }

    crow::json::wvalue ProgramWorker::startProgram(const std::string &program, const std::vector<std::string> &args) {
        std::lock_guard<std::mutex> lock(programLock);
        if (runningPid > 0) {
            return {{"status", "error"}, {"message", "Program already running"}};
        }

        int pipefd[2];
        if (pipe(pipefd) == -1) {
            return {{"status", "error"}, {"message", "Failed to create pipe"}};
        }

        pid_t pid = fork();
        if (pid == 0) {
            close(pipefd[0]);
            dup2(pipefd[1], STDOUT_FILENO);
            dup2(pipefd[1], STDERR_FILENO);
            close(pipefd[1]);

            std::vector<char*> cargs;
            cargs.push_back(const_cast<char*>(program.c_str()));
            for (const auto& a : args) cargs.push_back(const_cast<char*>(a.c_str()));
            cargs.push_back(nullptr);

            execvp(program.c_str(), cargs.data());
            _exit(1);
        } else if (pid > 0) {
            close(pipefd[1]);
            runningPid = pid;
            currentProgram = program;

            captureThread = std::thread([this, pipefd]() {
                char buffer[1024];
                ssize_t n;
                while ((n = read(pipefd[0], buffer, sizeof(buffer))) > 0) {
                    std::string msg(buffer, n);
                    sendWebSocket("{\"output\":\"" + escapeJson(msg) + "\"}");
                }
                close(pipefd[0]);

                std::lock_guard<std::mutex> lock(programLock);
                runningPid = -1;
                sendWebSocket("{\"status\":\"stopped\"}");
            });

            return {{"status", "started"}, {"pid", pid}};
        } else {
            return {{"status", "error"}, {"message", "Fork failed"}};
        }
    }

    crow::json::wvalue ProgramWorker::killProgram() {
        std::lock_guard<std::mutex> lock(programLock);

        if (runningPid <= 0) {
            return {{"status", "error"}, {"message", "No program running."}};
        }

        if (kill(runningPid, SIGKILL) == 0) {
            int status;
            waitpid(runningPid, &status, 0);
            runningPid = -1;
            sendWebSocket("{\"status\":\"killed\"}");
            return {{"status", "killed"}};
        } else {
            return {{"status", "error"}, {"message", "Failed to kill program"}};
        }
    }

    void ProgramWorker::status() {
        std::lock_guard<std::mutex> lock(programLock);

        if (runningPid > 0) {
            sendWebSocket("{\"status\":\"running\",\"program\":\"" + escapeJson(currentProgram)
                            + "\",\"pid\":" + std::to_string(runningPid.load()) + "}");
        } else {
            sendWebSocket("{\"status\":\"stopped\"}");
        }
    }
} // ProgramRunner