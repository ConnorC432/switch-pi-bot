//
// Created by connor on 24/09/2025.
//

#include "Database.h"
#include <iostream>

namespace Database {
    Database::Database(const std::string& host,
                       const std::string& user,
                       const std::string& password,
                       const std::string& dbName,
                       const std::string& authDB)
        : instance(),
        client(mongocxx::uri("mongodb://" + user + ":" + password + "@" + host + "/" + dbName + "?authSource=" + authDB))
        db(client[dbName])

    Database::~Database() {}

    Database& Database::getInstance(const std::string &host,
                                    const std::string &user,
                                    const std::string &password,
                                    const std::string &dbName,
                                    const std::string &authDB) {
        if (!instance) {
            std::lock_guard<std::mutex> lock(mutex);
            if (!instance) {
                instance = std::unique_ptr<Database>(
                        new Database(host, user, password, dbName, authDB)
                );
            }
        }
        return *instance;
    }

    mongocxx:collection Database::getCollection(const std::string& collectionName) {
        return db[collectionName];
    };
} // Database