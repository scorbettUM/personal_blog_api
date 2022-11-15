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
                    Subscribable(
                        "save_posts"
                    ),
                    posts_mapper(drogon::app().getDbClient())
                {

                }

                void initialize(Json::Value config){

                    logger = logger_factory.createConsoleLogger("console");
                    file_logger = logger_factory.createFileLogger("markdown_job", "blog.posts.job.log");

                };
                void run(){

                    int cache_idx = 0;
                    std::vector<std::string> processed;

                    for (const auto &filepath : cache.iter()){

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

                    LOG_DEBUG(logger, "Markdown -> HTML converter task: Saved: {} new articles.", save_count);
                    LOG_INFO(file_logger, "Markdown -> HTML converter task: Saved: {} new articles.", save_count); 

                    save_count = 0;

                };

                void stop(){

                };
                void complete(){

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
                
                drogon::orm::Mapper<drogon_model::sqlite3::Posts> posts_mapper;
                utilities::cache::QuickStore<std::string, std::pair<task::types::PostAction, std::string>> cache;
        };

    }
}
