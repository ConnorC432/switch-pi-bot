//
// Created by connor on 24/09/2025.
//

#include "Programs.h"
#include <iostream>

namespace Database {
    Programs::Programs(Database& db, const std::string& collectionName) {
        collection_ = db.getCollection(collectionName);
    }

    std::vector<Program> Programs::getPrograms() {
        std::vector<Program> programs;

        bson_t* query = bson_new();  // empty query = get all documents
        mongoc_cursor_t* cursor = mongoc_collection_find_with_opts(collection_, query, nullptr, nullptr);

        const bson_t* doc;
        while (mongoc_cursor_next(cursor, &doc)) {
            char* json_str = bson_as_canonical_extended_json(doc, nullptr);
            std::cout << "Document found: " << json_str << std::endl; // debug print
            bson_free(json_str);

            Program p;
            bson_iter_t iter;

            if (bson_iter_init_find(&iter, doc, "programName") && BSON_ITER_HOLDS_UTF8(&iter))
                p.programName = bson_iter_utf8(&iter, nullptr);

            if (bson_iter_init_find(&iter, doc, "displayName") && BSON_ITER_HOLDS_UTF8(&iter))
                p.displayName = bson_iter_utf8(&iter, nullptr);

            if (bson_iter_init_find(&iter, doc, "description") && BSON_ITER_HOLDS_UTF8(&iter))
                p.description = bson_iter_utf8(&iter, nullptr);

            if (bson_iter_init_find(&iter, doc, "category") && BSON_ITER_HOLDS_UTF8(&iter))
                p.category = bson_iter_utf8(&iter, nullptr);

            // Extract settings array safely
            if (bson_iter_init_find(&iter, doc, "settings") && BSON_ITER_HOLDS_ARRAY(&iter)) {
                bson_iter_t array_iter;
                if (bson_iter_recurse(&iter, &array_iter)) {
                    while (bson_iter_next(&array_iter)) {
                        const uint8_t* subdoc_data = nullptr;
                        uint32_t subdoc_len = 0;
                        bson_iter_document(&array_iter, &subdoc_len, &subdoc_data);
                        bson_t subdoc;
                        if (bson_init_static(&subdoc, subdoc_data, subdoc_len)) {
                            Setting s;
                            bson_iter_t field;
                            if (bson_iter_init_find(&field, &subdoc, "argName") && BSON_ITER_HOLDS_UTF8(&field))
                                s.argName = bson_iter_utf8(&field, nullptr);
                            if (bson_iter_init_find(&field, &subdoc, "displayName") && BSON_ITER_HOLDS_UTF8(&field))
                                s.displayName = bson_iter_utf8(&field, nullptr);
                            if (bson_iter_init_find(&field, &subdoc, "value")) {
                                if (BSON_ITER_HOLDS_INT32(&field))
                                    s.value = static_cast<int>(bson_iter_int32(&field));
                                else if (BSON_ITER_HOLDS_DOUBLE(&field))
                                    s.value = bson_iter_double(&field);
                                else if (BSON_ITER_HOLDS_UTF8(&field))
                                    s.value = std::string(bson_iter_utf8(&field, nullptr));
                                else if (BSON_ITER_HOLDS_BOOL(&field))
                                    s.value = bson_iter_bool(&field);
                            }
                            p.settings.push_back(s);
                        }
                    }
                }
            }

            programs.push_back(p);
        }

        // Debug: if no documents found
        if (programs.empty()) {
            std::cerr << "No programs found in collection." << std::endl;
        }

        bson_destroy(query);
        mongoc_cursor_destroy(cursor);

        return programs;
    }


    bool Programs::updatePrograms(const std::string& programName, const std::string& category,
                                  const Setting& setting) {
        bson_t* filter = bson_new();
        BSON_APPEND_UTF8(filter, "programName", programName.c_str());
        BSON_APPEND_UTF8(filter, "category", category.c_str());

        bson_t set_doc;
        bson_init(&set_doc);

        bson_t value_doc;
        bson_init(&value_doc);

        std::visit([&value_doc](auto&& v){
            using T = std::decay_t<decltype(v)>;
            if constexpr (std::is_same_v<T,int>)
                BSON_APPEND_INT32(&value_doc, "value", v);
            else if constexpr (std::is_same_v<T,double>)
                BSON_APPEND_DOUBLE(&value_doc, "value", v);
            else if constexpr (std::is_same_v<T,std::string>)
                BSON_APPEND_UTF8(&value_doc, "value", v.c_str());
            else if constexpr (std::is_same_v<T,bool>)
                BSON_APPEND_BOOL(&value_doc, "value", v);
        }, setting.value);

        // $set: settings.$[elem] = { value: ... }
        bson_t set_update;
        bson_init(&set_update);
        bson_append_document(&set_update, "settings.$[elem]", -1, &value_doc);

        bson_t update;
        bson_init(&update);
        bson_append_document(&update, "$set", -1, &set_update);

        // arrayFilters: [{"elem.argName": setting.argName}]
        bson_t opts;
        bson_init(&opts);
        bson_t arr;
        bson_init(&arr);
        bson_t filter_elem;
        bson_init(&filter_elem);
        BSON_APPEND_UTF8(&filter_elem, "elem.argName", setting.argName.c_str());
        bson_append_document(&arr, "0", -1, &filter_elem);
        BSON_APPEND_ARRAY(&opts, "arrayFilters", &arr);

        bson_error_t error;
        bool result = mongoc_collection_update_one(collection_, filter, &update, &opts, nullptr, &error);
        if (!result) {
            std::cerr << "Failed to update program: " << error.message << std::endl;
        }

        bson_destroy(filter);
        bson_destroy(&set_doc);
        bson_destroy(&value_doc);
        bson_destroy(&set_update);
        bson_destroy(&update);
        bson_destroy(&opts);
        bson_destroy(&arr);
        bson_destroy(&filter_elem);

        return result;
    }
} // Database