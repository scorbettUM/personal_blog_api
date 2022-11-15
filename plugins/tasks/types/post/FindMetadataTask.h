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

        class FindMetadataTask : public Subscribable<std::string, std::pair<task::types::PostAction, std::string>> {
            public:
                FindMetadataTask():
                    Subscribable(
                        "find_metadata"
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

                    std::vector<std::pair<std::string, std::string>> post_directories;
                    std::vector<std::string> processed;

                    for (const auto &filepath : cache.iter()){

                        auto cache_item = cache.get(filepath).value();

                        if (cache_item.first == PostAction::PROCESSED_POST){

                            std::string post_directory = std::filesystem::path(filepath).parent_path();

                            post_directories.push_back(
                                std::pair(
                                    post_directory,
                                    cache_item.second
                                )
                            );

                            processed.push_back(filepath);
                            
                        }
                    }

                    for (const auto &post_directory : post_directories){

                        for (auto &metadata_file : std::filesystem::recursive_directory_iterator(post_directory.first))
                        {

                            auto metadata_filename = metadata_file.path().filename().string();

                            if (metadata_filename.find(tags_ext)  != std::string::npos || metadata_filename.find(categories_ext)  != std::string::npos){

                                auto filepath = metadata_file.path().string();

                                utilities::cache::StoreResult result = cache.store(
                                    filepath, 
                                    std::pair(
                                        PostAction::LOAD_METADATA,
                                        post_directory.second
                                    )
                                );

                                send(
                                    filepath, 
                                    std::pair(
                                        PostAction::LOAD_METADATA,
                                        post_directory.second
                                    )
                                );

                                auto increment_amount = result == utilities::cache::StoreResult::CACHE_ITEM_CREATED ? 1 : 0;
                                metadata_files_discovered += increment_amount;


                            }
                        }

                    }

                    for (auto &metadata_filepath : processed){
                        cache.remove(metadata_filepath);
                    }

                    LOG_DEBUG(logger, "Markdown -> HTML converter task: Discovered: {} new metadata files.", metadata_files_discovered);
                    LOG_INFO(file_logger, "Markdown -> HTML converter task: Discovered: {} new metadata files.", metadata_files_discovered);
                    metadata_files_discovered = 0; 

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
                int metadata_files_discovered = 0; 
                utilities::cache::QuickStore<std::string, std::pair<task::types::PostAction, std::string>> cache;
        };

    }
}
