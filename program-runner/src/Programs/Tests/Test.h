//
// Created by connor on 21/09/2025.
//

#ifndef PROGRAMRUNNER_TEST_H
#define PROGRAMRUNNER_TEST_H

#include <vector>
#include <string>
#include <map>
#include "crow.h"

namespace Programs {
    class Tests {
    public:
        class Test {
        public:
            static void run(const std::map<std::string, crow::json::rvalue> &args);
        };
    };
} // Programs

#endif //PROGRAMRUNNER_TEST_H
