//
// Created by connor on 21/09/2025.
//

#ifndef PROGRAMRUNNER_PROGRAMREGISTRY_H
#define PROGRAMRUNNER_PROGRAMREGISTRY_H

#pragma once
#include <string>
#include <vector>
#include <functional>
#include <map>
#include <crow.h>

namespace ProgramRunner {
    class ProgramRegistry {
    private:
        using ProgramFunc = std::function<void(const std::map<std::string, crow::json::rvalue>)>;

        std::map<std::string, std::map<std::string, ProgramFunc>> programs;

    public:
        static ProgramRegistry& instance();

        void init();

        ///
        /// \param category Program Category
        /// \param name Program Name
        /// \param func Program Function
        void registerProgram(const std::string& category, const std::string& name, ProgramFunc func);

        ///
        /// \param category Program Category
        /// \param name Program Name
        /// \return Program Function
        ProgramFunc getProgram(const std::string& category, const std::string& name);
    };
} // ProgramRunner

#endif //PROGRAMRUNNER_PROGRAMREGISTRY_H
