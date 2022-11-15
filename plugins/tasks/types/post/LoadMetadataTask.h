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
                    Subscribable<std::string, std::pair<task::types::PostAction, std::string>>(
                        "load_metadata",
                        100,
                        5,
                        std::vector<std::string>{
                            "save_tags",
                            "save_categories"
                        }
                    )
                {

                }

                void initialize(Json::Value config){

                    task_logger = logger_factory.createConsoleLogger("console");
                    task_file_logger = logger_factory.createFileLogger("load_metadata", "blog.jobs.log");

                    LOG_DEBUG(task_logger, "Initializing {} task. Please wait...", task_name);
                    LOG_INFO(task_file_logger, "Initializing {} task. Please wait...", task_name);

                };

                void run(){

                    auto tags_ext = std::string(".tags.csv");
                    auto categories_ext = std::string(".categories.csv");

                    int cache_idx = 0;
                    std::vector<std::string> processed;

                    for (std::string &filepath : cache.iter()){

                        auto cache_item = cache.get(filepath).value();
                        auto metadata_filepath = std::filesystem::path(filepath);

                        auto metadata_filename = metadata_filepath.filename().string();


                        auto cache_action = metadata_filename.find(tags_ext) != std::string::npos ? PostAction::SAVE_TAGS : PostAction::SAVE_CATEGORIES;

                        std::cout<<filepath<<std::endl;

                        if (cache_item.first == PostAction::LOAD_METADATA){

                            std::stringstream buffer;
                            std::ifstream ifs(filepath);       // note no mode needed

                            if ( !ifs.is_open() ) { 

                                error_filepath = metadata_filepath.string();
                                
                                LOG_WARNING(task_logger, "{} task: Failed to open metadata file at: {}", task_name, error_filepath);  
                                LOG_WARNING(task_file_logger, "{} task: Failed to open metadata file at: {}", task_name, error_filepath);   

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


                    LOG_DEBUG(task_logger, "{} task: Loaded: {} new metadata files.", task_name, metadata_files_loaded);
                    LOG_INFO(task_file_logger, "{} task: Loaded: {} new metadata files.", task_name, metadata_files_loaded);
                    metadata_files_loaded = 0;

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

                int metadata_files_loaded = 0;
                std::string error_filepath;
                utilities::cache::QuickStore<std::string, std::pair<task::types::PostAction, std::string>> cache;
                utilities::logging::LoggerFactory logger_factory;
                quill::Logger *task_logger;
                quill::Logger *task_file_logger;
        };

    }
}
