//
// Created by connor on 21/09/2025.
//

#include "ProgramRegistry.h"

// Program Header Files
#include "Programs/Test.h"

namespace ProgramRunner {
    ProgramRegistry& ProgramRegistry::instance() {
        static ProgramRegistry registry;
        return registry;
    }

    void ProgramRegistry::registerProgram(const std::string &name,
                                          ProgramRunner::ProgramRegistry::ProgramFunc func) {
        programs[name] = std::move(func);
    }

    ProgramRegistry::ProgramFunc ProgramRegistry::getProgram(const std::string &name) {
        auto it = programs.find(name);
        if (it != programs.end()) {
            return it->second;
        }
        return nullptr;
    }

    void ProgramRegistry::init() {
        registerProgram("Test", Programs::Test::run);
    }
} // ProgramRunner