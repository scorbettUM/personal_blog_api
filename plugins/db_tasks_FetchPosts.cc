/**
 *
 *  db_tasks_FetchPosts.cc
 *
 */

#include "db_tasks_FetchPosts.h"
#include <drogon/drogon.h>
#include <models/Posts.h>
#include <stdlib.h>
#include <cstdlib>
#include <iostream>
#include <fstream>

using namespace drogon;
using namespace drogon_model;
using namespace db::tasks;


void FetchPosts::getNewPosts(){
    auto http_client = drogon::HttpClient::newHttpClient("https://httpbin.org");
    auto req = drogon::HttpRequest::newHttpRequest();
        req->setMethod(drogon::Get);
        req->setPath("/get");

    auto articles_json = req->jsonObject();

    auto db = drogon::app().getDbClient();

    drogon::orm::Mapper<drogon_model::sqlite3::Posts> posts_mapper(db);

    auto response = posts_mapper.findAll();

    std::cout<<"Retrieved - "<<response.size()<<" - posts."<<std::endl;
}

void FetchPosts::initAndStart(const Json::Value &config)
{
    /// Initialize and start the plugin
    auto db = drogon::app().getDbClient();

    auto database_path = getenv("SQLITE_DATABASE_PATH");
    if (database_path == NULL) {
        database_path = (char *)"../db/blog.db";
    }

    if (std::ifstream(database_path)){
        std::cout<<"Database file exists! No need to create that."<<std::endl;
    } else {
        std::ofstream database_file(database_path);
        if (!database_file) {
            std::cout<<"Err. - could not create DB file."<<std::endl;
        }
    }

    auto sql = "CREATE TABLE IF NOT EXISTS Posts(" \
        "id        INT        PRIMARY KEY NOT NULL," \
        "name      TEXT       NOT NULL," \
        "body      TEXT       NOT NULL," \
        "created   DATETIME   DEFAULT CURRENT_TIMESTAMP," \
        "updated   DATETIME   DEFAULT CURRENT_TIMESTAMP);"; 

    db->execSqlAsync(sql, [&](const drogon::orm::Result &result) {

        std::cout<<"Create or reinitialized Posts table successfully!"<<std::endl;

    }, [&](const drogon::orm::DrogonDbException &db_error) {
        const auto exception = db_error.base();

        std::cout<<"Encountered error create Posts table - "<< exception.what() << "." << std::endl;
    });
}

void FetchPosts::shutdown() 
{
    /// Shutdown the plugin
}
