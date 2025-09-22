//
// Created by connor on 21/09/2025.
//

#include "Test.h"
#include <iostream>
#include <thread>
#include <chrono>
#include "crow.h"

namespace Programs {
    void Tests::Test::run(const std::map<std::string, crow::json::rvalue> &args) {
        int waitTime = 0;
        auto it = args.find("waitTime");
        if (it != args.end() && it->second.t() == crow::json::type::Number) {
            waitTime = it->second.i();
        }

        std::cout << waitTime << std::endl;

        std::this_thread::sleep_for(std::chrono::seconds(waitTime));

        std::cout << "Test Program Success" << std::endl;
    }
} // Programs