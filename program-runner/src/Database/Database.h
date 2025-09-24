//
// Created by connor on 24/09/2025.
//

#ifndef SWITCH_PI_BOT_DATABASE_H
#define SWITCH_PI_BOT_DATABASE_

#pragma once
#include <mongoc.h>
#include <string>

namespace Database {
    class Database {
    private:
        mongoc_client_t* client_;
        std::string databaseName_;

    public:
        Database(const std::string& host = "mongodb",
                 const std::string& user = "admin",
                 const std::string& pass = "root",
                 const std::string& dbName = "switchPiBot",
                 const std::string& authDB = "admin"
                 );
        ~Database();

        mongoc_collection_t* getCollection(const std::string& collectionName);
    };
} // Database

#endif //SWITCH_PI_BOT_DATABASE_H
