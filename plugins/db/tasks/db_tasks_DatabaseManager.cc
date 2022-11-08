/**
 *
 *  db_tasks_DatabaseManager.cc
 *
 */

#include "db_tasks_DatabaseManager.h"
#include <drogon/drogon.h>
#include <models/Posts.h>
#include <stdlib.h>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <time.h>
#include <cstdlib>
#include <quill/Quill.h>

using namespace drogon;
using namespace db::tasks;

void DatabaseManager::initAndStart(const Json::Value &config)
{
    /// Initialize and start the plugin
    auto db = drogon::app().getDbClient();

    auto logger = quill::get_logger();

    auto api_database_path = std::string("../db/blog.db");
    auto database_path = getenv("SQLITE_DATABASE_PATH");
    if (database_path != NULL) {
        api_database_path = std::string(database_path);
        
    } else if (!config["database_path"].isNull()){
        api_database_path = config["database_path"].asString();

    }
    
    LOG_INFO(logger, "Database Manager connecting to database at: {}", api_database_path);

    if (std::ifstream(api_database_path)){
        LOG_INFO(logger, "Database at: {} exists. Skipping creation.", api_database_path);
    } else {
        std::ofstream database_file(api_database_path);
        if (!database_file) {

            LOG_CRITICAL(logger, "Err. - could not create or connect database file at {}", api_database_path);
        }
    }

    auto sql = "CREATE TABLE IF NOT EXISTS Posts(" \
        "post_id   TEXT       PRIMARY KEY NOT NULL," \
        "name      TEXT       NOT NULL," \
        "body      TEXT       NOT NULL," \
        "created   DATETIME   DEFAULT CURRENT_TIMESTAMP," \
        "updated   DATETIME   DEFAULT CURRENT_TIMESTAMP);"; 

    db->execSqlAsync(sql, [&](const drogon::orm::Result &result) {

        assert(result.size() == 0);


    }, [&](const drogon::orm::DrogonDbException &db_error) {
        const auto exception = db_error.base();

        LOG_CRITICAL(logger, "Encountered error creating or connecting to Posts table: {}", exception.what());

    });

    LOG_INFO(logger, "Successfully created or connected to Posts table.");
}

void DatabaseManager::shutdown() 
{
    /// Shutdown the plugin
}
