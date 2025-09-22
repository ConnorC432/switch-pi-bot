//
// Created by connor on 21/09/2025.
//

#include "ProgramWorker.h"
#include "ProgramRegistry.h"
#include <sys/wait.h>
#include <signal.h>
#include <iostream>
#include <crow.h>

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

    void ProgramWorker::bindStatusWebSocket(crow::websocket::connection *conn) {
        std::lock_guard<std::mutex> lock(statusWsLock);
        statusWs = conn;
    }

    void ProgramWorker::unbindStatusWebSocket() {
        std::lock_guard<std::mutex> lock(statusWsLock);
        statusWs = nullptr;
    }

    void ProgramWorker::bindOutputWebSocket(crow::websocket::connection *conn) {
        std::lock_guard<std::mutex> lock(outputWsLock);
        outputWs = conn;
    }

    void ProgramWorker::unbindOutputWebSocket() {
        std::lock_guard<std::mutex> lock(outputWsLock);
        outputWs = nullptr;
    }

    void ProgramWorker::sendStatus(const std::string& message) {
        std::lock_guard<std::mutex> lock(statusWsLock);
        if (statusWs) {
            try { statusWs->send_text(message); }
            catch(...) { statusWs = nullptr; }
        }
    }

    void ProgramWorker::sendOutput(const std::string& message) {
        std::lock_guard<std::mutex> lock(outputWsLock);
        if (outputWs) {
            try { outputWs->send_text(message); }
            catch(...) { outputWs = nullptr; }
        }
    }

    crow::json::wvalue ProgramWorker::startProgram(const std::string &category,
                                                   const std::string &program,
                                                   const std::map<std::string, crow::json::rvalue> &args) {
        std::lock_guard<std::mutex> lock(programLock);

        if (runningPid > 0) {
            return {{"status", "error"}, {"message", "Program already running"}};
        }

        if (captureThread.joinable()) {
            captureThread.join();
        }

        auto func = ProgramRegistry::instance().getProgram(category, program);
        if (!func) {
            return {{"status", "error"}, {"message", "Program not found"}};
        }

        runningPid = 1;
        currentProgram = program;

        class WebSocketStreamBuf : public std::streambuf {
        ProgramWorker* worker;
        std::string buffer;
        public:
            explicit WebSocketStreamBuf(ProgramWorker* w) : worker(w) {}

        protected:
            virtual int_type overflow(int_type c) override {
                if (c != EOF) {
                    char ch = static_cast<char>(c);
                    buffer += ch;
                    if (ch == '\n') { // flush line on newline
                        worker->sendOutput(worker->escapeJson(buffer));
                        buffer.clear();
                    }
                }
                return c;
            }

            virtual int sync() override {
                if (!buffer.empty()) {
                    worker->sendOutput(worker->escapeJson(buffer));
                    buffer.clear();
                }
                return 0;
            }
        };

        captureThread = std::thread([this, func, args]() {
            WebSocketStreamBuf wsbuf(this);
            std::ostream wsout(&wsbuf);

            std::streambuf* oldCout = std::cout.rdbuf(wsout.rdbuf());
            std::streambuf* oldCerr = std::cerr.rdbuf(wsout.rdbuf());

            try {
                func(args);
            } catch (const std::exception &e) {
                std::cerr << "Exception: " << e.what() << std::endl;
            }

            std::cout.rdbuf(oldCout);
            std::cerr.rdbuf(oldCerr);

            wsout.flush();

            {
                std::lock_guard<std::mutex> lock(programLock);
                runningPid = -1;
                sendStatus("{\"status\":\"stopped\"}");
            }
        });

        return {{"status", "started"}};
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
            sendStatus("{\"status\":\"killed\"}");
            return {{"status", "killed"}};
        } else {
            return {{"status", "error"}, {"message", "Failed to kill program"}};
        }
    }
} // ProgramRunner