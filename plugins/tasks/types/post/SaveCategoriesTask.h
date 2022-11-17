#pragma once

#include <string>
#include <cstdlib>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <stdlib.h>
#include <map>
#include <drogon/drogon.h>
#include <models/Categories.h>

#include <utilities/filesystem/paths.h>

#ifndef POSTS_COMMON
#define POSTS_COMMON
#include "PostAction.h"
#include <utilities/uuid/uuid.h>
#include <utilities/string/string_utils.h>
#include <utilities/storage/quick_store.h>
#include <plugins/tasks/types/base/Subscribable.h>
#endif




using namespace drogon;
using namespace drogon_model;
using namespace messaging::pubsub;

namespace task {

    namespace types {

        class SaveCategoriesTask : public Subscribable<std::string, std::pair<task::types::PostAction, std::string>> {
            public:
                SaveCategoriesTask():
                
                    Subscribable<std::string, std::pair<task::types::PostAction, std::string>>(
                        "save_categories",
                        100,
                        7,
                        std::vector<std::string>{}
                    )
                {

                }

                void initialize(Json::Value config){

                    (void)config;

                    task_logger = logger_factory.createConsoleLogger("console");
                    task_file_logger = logger_factory.createFileLogger("save_categories", "blog.jobs.log");

                    LOG_DEBUG(task_logger, "Initializing {} task. Please wait...", task_name);
                    LOG_INFO(task_file_logger, "Initializing {} task. Please wait...", task_name);

                };
                void run(){

                    int cache_idx = 0;
                    std::vector<std::string> processed;

                    auto db = drogon::app().getDbClient();
                    drogon::orm::Mapper<drogon_model::sqlite3::Categories> categories_mapper(db);

                    for (std::string &metadata_content : cache.iter()){

                        auto cache_item = cache.get(metadata_content).value();
                        if (cache_item.first == PostAction::SAVE_CATEGORIES){

                            for (std::string &metadata_item : utilities::string::split(metadata_content, ',')){

                                auto metadata_id = uuid::generate_uuid_v4();
                                auto record_count = categories_mapper.countFuture(
                                    drogon::orm::Criteria("name", drogon::orm::CompareOperator::EQ, metadata_item) &&
                                    drogon::orm::Criteria("post", drogon::orm::CompareOperator::EQ, cache_item.second)
                                ).get();

                                if (record_count == 0){

                                    Json::Value data;
                                    data["category_id"] = metadata_id;
                                    data["name"] = metadata_item;
                                    data["post"] = cache_item.second;

                                    auto category = drogon_model::sqlite3::Categories(data);
                                    categories_mapper.insertFuture(category).get();

                                    categories_saved += 1;

                                    processed.push_back(metadata_item);

                                }


                            }

                        }

                        cache_idx += 1;

                    }

                    for (auto &metadata_item : processed){
                        cache.remove(metadata_item);
                    }

                    LOG_DEBUG(task_logger, "{} task: Saved: {} new categories.", task_name, categories_saved);
                    LOG_DEBUG(task_file_logger, "{} task: Saved: {} new categories.", task_name, categories_saved);
                    
                    categories_saved = 0;


                };

                void stop(){
                    
                    LOG_DEBUG(task_logger, "{} task has been notified of shutdown. Please wait...", task_name);
                    LOG_INFO(task_file_logger, "{} task has been notified of shutdown. Please wait...", task_name);

                };
                void complete(){

                    LOG_DEBUG(task_logger, "{} task has completed.", task_name);
                    LOG_INFO(task_file_logger, "{} task has completed.", task_name);

                };
                void receive(std::string key, std::pair<task::types::PostAction, std::string> value){

                    cache.store(
                        key,
                        value
                    );
                }

            private:
                int categories_saved = 0;
                utilities::cache::QuickStore<std::string, std::pair<task::types::PostAction, std::string>> cache;
                utilities::logging::LoggerFactory logger_factory;
                quill::Logger *task_logger;
                quill::Logger *task_file_logger;
        };

    }
}
