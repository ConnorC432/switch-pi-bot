//
// Created by connor on 21/09/2025.
//

#include "ProgramRegistry.h"

// Program Header Files
#include "Programs/Tests/Test.h"

namespace ProgramRunner {
    ProgramRegistry& ProgramRegistry::instance() {
        static ProgramRegistry registry;
        return registry;
    }

    void ProgramRegistry::registerProgram(const std::string &category,
                                          const std::string &name,
                                          ProgramRunner::ProgramRegistry::ProgramFunc func) {
        programs[category][name] = std::move(func);
    }

    ProgramRegistry::ProgramFunc ProgramRegistry::getProgram(const std::string &category,
                                                             const std::string &name) {
        auto catIt = programs.find(category);
        if (catIt != programs.end()) {
            auto progIt = catIt->second.find(name);
            if (progIt != catIt->second.end()) {
                return progIt->second;
            }
        }
        return nullptr;
    }

    void ProgramRegistry::init() {
        registerProgram("Tests", "Test", Programs::Tests::Test::run);
    }
} // ProgramRunner