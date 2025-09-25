//
// Created by connor on 24/09/2025.
//

#ifndef SWITCH_PI_BOT_DATABASE_H
#define SWITCH_PI_BOT_DATABASE_H

#pragma once
#include <string>
#include <mutex>
#include <memory>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/database.hpp>
#include <mongocxx/collection.hpp>

namespace Database {
    class Database {
    private:
        mongocxx::instance instance;
        mongocxx::client client;
        mongocxx::database db;

        static std::unique_ptr<Database> instance;
        static std::mutex mutex;

        Database(const std::string& host,
                 const std::string& user,
                 const std::string& password,
                 const std::string& dbName,
                 const std::string& authDB);

    public:
        Database(const Database&) = delete;
        Database& operator=(const Database&) = delete;

        ~Database();

        static Database& getInstance(const std::string& host = "mongodb",
                                     const std::string& user = "admin",
                                     const std::string& password = "root",
                                     const std::string& dbName = "switchPiBot",
                                     const std::string& authDB = "admin");

        mongocxx::collection getCollection(const std::string& collectionName);
    };
} // Database

#endif //SWITCH_PI_BOT_DATABASE_H
