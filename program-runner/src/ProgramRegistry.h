//
// Created by connor on 21/09/2025.
//

#ifndef PROGRAMRUNNER_PROGRAMREGISTRY_H
#define PROGRAMRUNNER_PROGRAMREGISTRY_H

#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <map>

namespace ProgramRunner {
    class ProgramRegistry {
    private:
        using ProgramFunc = std::function<void(const std::vector<std::string>&)>;

        std::unordered_map<std::string, ProgramFunc> programs;

    public:
        static ProgramRegistry& instance();

        void init();

        void registerProgram(const std::string& name, ProgramFunc func);
        ProgramFunc getProgram(const std::string& name);
    };
} // ProgramRunner

#endif //PROGRAMRUNNER_PROGRAMREGISTRY_H
