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
#include "MetadataType.h"
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

        class LoadMetadataTask : public Subscribable<std::string, std::pair<task::types::PostAction, std::string>> {
            public:
                LoadMetadataTask():
                    Subscribable(
                        "load_metadata"
                    )
                {

                }

                void initialize(Json::Value config){

                    logger = logger_factory.createConsoleLogger("console");
                    file_logger = logger_factory.createFileLogger("markdown_job", "blog.posts.job.log");


                };

                void run(){

                    auto tags_ext = std::string(".tags.csv");
                    auto categories_ext = std::string(".categories.csv");

                    int cache_idx = 0;
                    std::vector<std::string> processed;

                    for (const auto &filepath : cache.iter()){

                        auto cache_item = cache.get(filepath).value();

                        auto metadata_filename = std::filesystem::path(filepath).filename().string();


                        auto cache_action = metadata_filename.find(tags_ext) != std::string::npos ? PostAction::SAVE_TAGS : PostAction::SAVE_CATEGORIES;

                        if (cache_item.first == PostAction::LOAD_METADATA){

                            std::stringstream buffer;
                            std::ifstream ifs(filepath);       // note no mode needed

                            if ( !ifs.is_open() ) { 
                                LOG_WARNING(logger, "Markdown -> HTML converter task: Failed to open tags file at: {}", filepath);  
                                LOG_WARNING(file_logger, "Markdown -> HTML converter task: Failed to open tags file at: {}", filepath);   

                            }
                            else {
                    
                                buffer << ifs.rdbuf();

                                auto buffer_string = buffer.str();
                                utilities::cache::StoreResult result = cache.store(
                                    buffer_string,
                                    std::pair(
                                        cache_action,
                                        cache_item.second
                                    )
                                );

                                send(
                                    buffer_string,
                                    std::pair(
                                        cache_action,
                                        cache_item.second
                                    )
                                );


                                processed.push_back(filepath);

                                auto increment_amount = result == utilities::cache::StoreResult::CACHE_ITEM_CREATED ? 1 : 0;
                                metadata_files_loaded += increment_amount;

                                ifs.close();

                            }

                        }

                        cache_idx += 1;

                    }

                    for (auto &filepath : processed){
                        cache.remove(filepath);
                    }


                    LOG_DEBUG(logger, "Markdown -> HTML converter task: Loaded: {} new metadata files.", metadata_files_loaded);
                    LOG_INFO(file_logger, "Markdown -> HTML converter task: Loaded: {} new metadata files.", metadata_files_loaded);
                    metadata_files_loaded = 0;

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

                int metadata_files_loaded = 0;
                utilities::cache::QuickStore<std::string, std::pair<task::types::PostAction, std::string>> cache;
        };

    }
}
