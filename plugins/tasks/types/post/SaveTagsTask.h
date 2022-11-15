#pragma once

#include <string>
#include <cstdlib>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <stdlib.h>
#include <map>
#include <drogon/drogon.h>
#include <models/Tags.h>

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

        class SaveTagsTask : public Subscribable<std::string, std::pair<task::types::PostAction, std::string>> {
            public:
                SaveTagsTask():
                    Subscribable<std::string, std::pair<task::types::PostAction, std::string>>(
                        "save_tags",
                        100,
                        6,
                        std::vector<std::string>{}
                    ),
                    tags_mapper(drogon::app().getDbClient())
                {

                }

                void initialize(Json::Value config){

                    task_logger = logger_factory.createConsoleLogger("console");
                    task_file_logger = logger_factory.createFileLogger("save_tags", "blog.jobs.log");

                    LOG_DEBUG(task_logger, "Initializing {} task. Please wait...", task_name);
                    LOG_INFO(task_file_logger, "Initializing {} task. Please wait...", task_name);

                };
                void run(){
                    
                    int cache_idx = 0;
                    std::vector<std::string> processed;

                    for (std::string &metadata_content : cache.iter()){

                        auto cache_item = cache.get(metadata_content).value();
                        if (cache_item.first == PostAction::SAVE_TAGS){

                            for (std::string &metadata_item : utilities::string::split(metadata_content, ',')){

                                auto metadata_id = uuid::generate_uuid_v4();
                                auto record_count = tags_mapper.countFuture(
                                    drogon::orm::Criteria("name", drogon::orm::CompareOperator::EQ, metadata_item) &&
                                    drogon::orm::Criteria("post", drogon::orm::CompareOperator::EQ, cache_item.second)
                                ).get();

                                if (record_count == 0){

                                    Json::Value data;
                                    data["tag_id"] = metadata_id;
                                    data["name"] = metadata_item;
                                    data["post"] = cache_item.second;

                                    auto tag = drogon_model::sqlite3::Tags(data);
                                    tags_mapper.insertFuture(tag).get();

                                    tags_saved += 1;

                                    processed.push_back(metadata_item);

                                }

                            }

                        }

                        cache_idx += 1;
                    }

                    for (auto &metadata_item : processed){
                        cache.remove(metadata_item);
                    }

                    LOG_DEBUG(task_logger, "{} task: Saved: {} new tags.", task_name, tags_saved);
                    LOG_INFO(task_file_logger, "{} task: Saved: {} new tags.", task_name, tags_saved);

                    tags_saved = 0;


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
                int tags_saved = 0;
                
                drogon::orm::Mapper<drogon_model::sqlite3::Tags> tags_mapper;
                utilities::cache::QuickStore<std::string, std::pair<task::types::PostAction, std::string>> cache;
                utilities::logging::LoggerFactory logger_factory;
                quill::Logger *task_logger;
                quill::Logger *task_file_logger;
        };

    }
}
