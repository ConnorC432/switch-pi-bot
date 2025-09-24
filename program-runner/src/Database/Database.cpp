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
        : databaseName_(dbName) {
        mongoc_init();

        std::string uri;

        if (!user.empty() && !password.empty()) {
            // Construct: mongodb://user:pass@host:27017
            uri = "mongodb://" + user + ":" + password + "@" + host + ":27017";
            uri += "/?authSource=" + authDB;
        } else {
            uri = "mongodb://" + host + ":27017";
        }

        client_ = mongoc_client_new(uri.c_str());
        if (!client_) {
            std::cerr << "Failed to initialize MongoDB client with URI: " << uri << std::endl;
        } else {
            std::cout << "MongoDB client initialized with URI: " << uri << std::endl;
        }

        client_ = mongoc_client_new(uri.c_str());
        if (!client_) {
            std::cerr << "Failed to initialise MongoDB client" << std::endl;
        }
    }

    Database::~Database() {
        mongoc_client_destroy(client_);
        mongoc_cleanup();
    }

    mongoc_collection_t* Database::getCollection(const std::string &collectionName) {
        return mongoc_client_get_collection(client_, databaseName_.c_str(), collectionName.c_str());
    }
} // Database