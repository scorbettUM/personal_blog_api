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
                    Subscribable(
                        "save_tags"
                    ),
                    tags_mapper(drogon::app().getDbClient())
                {

                }

                void initialize(Json::Value config){

                    logger = logger_factory.createConsoleLogger("console");
                    file_logger = logger_factory.createFileLogger("markdown_job", "blog.posts.job.log");

                };
                void run(){
                    
                    int cache_idx = 0;
                    std::vector<std::string> processed;

                    for (const auto &metadata_content : cache.iter()){

                        auto cache_item = cache.get(metadata_content).value();
                        if (cache_item.first == PostAction::SAVE_TAGS){

                            for (const auto &metadata_item : utilities::string::split(metadata_content, ',')){

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
                                    
                                    send(
                                        metadata_item,
                                        std::pair(
                                            PostAction::PROCESSED_TAGS,
                                            std::string("OK")
                                        )
                                    );


                                    processed.push_back(metadata_item);

                                }

                            }

                        }

                        cache_idx += 1;
                    }

                    for (auto &metadata_item : processed){
                        cache.remove(metadata_item);
                    }

                    LOG_DEBUG(logger, "Markdown -> HTML converter task: Saved: {} new tags.", tags_saved);
                    LOG_INFO(file_logger, "Markdown -> HTML converter task: Saved: {} new tags.", tags_saved);

                    tags_saved = 0;


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
                int tags_saved = 0;
                
                drogon::orm::Mapper<drogon_model::sqlite3::Tags> tags_mapper;
                utilities::cache::QuickStore<std::string, std::pair<task::types::PostAction, std::string>> cache;
        };

    }
}
