//
// Created by connor on 24/09/2025.
//

#ifndef SWITCH_PI_BOT_PROGRAMS_H
#define SWITCH_PI_BOT_PROGRAMS_H

#pragma once

#include "Database.h"
#include <bson.h>
#include <mongoc.h>
#include <string>
#include <vector>
#include <variant>

namespace Database {
    struct Setting {
        std::string argName;
        std::string displayName;
        std::variant<int, double, std::string, bool> value;
    };

    struct Program {
        std::string programName;
        std::string displayName;
        std::string description;
        std::string category;
        std::vector<Setting> settings;
    };

    class Programs {
    private:
        mongoc_collection_t* collection_;

    public:
        Programs(Database& db, const std::string& collectionName);

        std::vector<Program> getPrograms();

        bool updatePrograms(const std::string& programName, const std::string& category,
                            const Setting& setting);
    };
} // Database

#endif //SWITCH_PI_BOT_PROGRAMS_H
