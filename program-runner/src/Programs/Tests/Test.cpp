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
        auto itTime = args.find("waitTime");
        if (itTime != args.end() && itTime->second.t() == crow::json::type::Number) {
            waitTime = itTime->second.i();
        }

        std::string testString;
        auto itString = args.find("testString");
        if (itString != args.end() && itString->second.t() == crow::json::type::String) {
            testString = itString->second.s();
        }

        std::cout << testString << std::endl;

        std::this_thread::sleep_for(std::chrono::seconds(waitTime));

        std::cout << "Test Program Success" << std::endl;
    }
} // Programs