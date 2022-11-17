#pragma once

#include <string>
#include <cstdlib>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <stdlib.h>
#include <map>
#include <drogon/drogon.h>
#include <models/Posts.h>

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

        class SavePostsTask : public Subscribable<std::string, std::pair<task::types::PostAction, std::string>> {
            public:
                SavePostsTask():
                    Subscribable<std::string, std::pair<task::types::PostAction, std::string>>(
                        "save_posts",
                        100,
                        3,
                        std::vector<std::string>{
                            "find_metadata"
                        }
                    )
                {

                }

                void initialize(Json::Value config){

                    (void)config;

                    task_logger = logger_factory.createConsoleLogger("console");
                    task_file_logger = logger_factory.createFileLogger("save_posts", "blog.jobs.log");

                    LOG_DEBUG(task_logger, "Initializing {} task. Please wait...", task_name);
                    LOG_INFO(task_file_logger, "Initializing {} task. Please wait...", task_name);

                };
                void run(){

                    int cache_idx = 0;
                    std::vector<std::string> processed;

                    auto db = drogon::app().getDbClient();
                    drogon::orm::Mapper<drogon_model::sqlite3::Posts> posts_mapper(db);

                    for (std::string &filepath : cache.iter()){

                        auto cache_item = cache.get(filepath).value();

                        if (cache_item.first == PostAction::SAVE_POST){

                            std::string post_body = cache_item.second;

                            auto path = std::filesystem::path(filepath);
                            std::string stub = path.stem().string();
                            std::string name = std::filesystem::path(stub).stem().string();

                            auto post_id = uuid::generate_uuid_v4();

                            Json::Value post_data;
                            post_data["post_id"] = post_id;
                            post_data["name"] = name;
                            post_data["body"] = post_body;
                            
                            auto post = drogon_model::sqlite3::Posts(post_data);
                            posts_mapper.insertFuture(post).get();


                            processed.push_back(filepath);

                            send(
                                filepath,
                                std::pair(
                                    PostAction::PROCESSED_POST,
                                    post_id
                                )
                            );

                            save_count += 1;
                        
                        }

                        cache_idx += 1;
                        
                    }

                    for (auto &filepath : processed){
                        cache.remove(filepath);
                    }

                    LOG_DEBUG(task_logger, "{} task: Saved: {} new articles.", task_name, save_count);
                    LOG_DEBUG(task_file_logger, "{} task: Saved: {} new articles.", task_name, save_count); 

                    save_count = 0;

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
                int article_process_interval = 60;
                int save_count = 0;
                std::string repo_path;
                
                utilities::cache::QuickStore<std::string, std::pair<task::types::PostAction, std::string>> cache;
                utilities::logging::LoggerFactory logger_factory;
                quill::Logger *task_logger;
                quill::Logger *task_file_logger;
        };

    }
}
