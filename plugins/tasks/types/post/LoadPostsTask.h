#pragma once

#include <string>
#include <cstdlib>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <stdlib.h>
#include <map>


#include "parser/parser.h"


#include <utilities/filesystem/paths.h>

#ifndef POSTS_COMMON
#define POSTS_COMMON
#include "PostAction.h"
#include <utilities/uuid/uuid.h>
#include <utilities/string/string_utils.h>
#include <utilities/storage/quick_store.h>
#include <plugins/tasks/types/base/Subscribable.h>
#include <plugins/tasks/types/base/BaseTask.h>
#endif


using namespace messaging::pubsub;

namespace task {

    namespace types {

        class LoadPostsTask : public Subscribable<std::string, std::pair<task::types::PostAction, std::string>> {
            public:
                LoadPostsTask():
                    Subscribable<std::string, std::pair<task::types::PostAction, std::string>>(
                        "load_posts",
                        100,
                        2,
                        std::vector<std::string>{
                            "save_posts"
                        }
                    ),
                    maddy_config(std::make_shared<maddy::ParserConfig>()),
                    parser(std::make_shared<maddy::Parser>(maddy_config))
                {

                }

                void initialize(Json::Value config){

                    task_logger = logger_factory.createConsoleLogger("console");
                    task_file_logger = logger_factory.createFileLogger("load_posts", "blog.jobs.log");

                    maddy_config = std::make_shared<maddy::ParserConfig>();
                    maddy_config->isEmphasizedParserEnabled = true; // default
                    maddy_config->isHTMLWrappedInParagraph = true; // default

                    parser = std::make_shared<maddy::Parser>(maddy_config);

                    LOG_DEBUG(task_logger, "Initializing {} task. Please wait...", task_name);
                    LOG_INFO(task_file_logger, "Initializing {} task. Please wait...", task_name);
                    
                };
                void run(){

                    int cache_idx = 0;
                    std::vector<std::string> processed;

                    for (std::string &filepath: cache.iter()){

                        auto cache_item = cache.get(filepath).value();

                        if (cache_item.first == PostAction::LOAD_POST){
                            std::stringstream buffer;
                            std::ifstream ifs(filepath);
                            std::string post_body;

                            auto path = std::filesystem::path(filepath);
                            std::string file_type = path.extension();

                            if ( !ifs.is_open() ) { 

                                error_filepath = path.string();

                                LOG_WARNING(task_logger, "{} task: Failed to open article at: {}", task_name, error_filepath);  
                                LOG_WARNING(task_file_logger, "{} task: Failed to open article at: {}", task_name, error_filepath);   

                            }
                            else {

                                buffer << ifs.rdbuf(); 
                                
                            }

                            if (file_type == ".md"){

                                post_body = parser->Parse(buffer);

                            }  else {

                                post_body = buffer.str();

                            }

                            processed.push_back(filepath);

                            send(
                                filepath,
                                std::pair(
                                    PostAction::SAVE_POST,
                                    post_body
                                )
                            );

                            loaded_count += 1;

                        }

                        cache_idx += 1;

                    }

                    for (auto &filepath : processed){
                        cache.remove(filepath);
                    }


                    LOG_DEBUG(task_logger, "{} task: Loaded: {} new articles.", task_name, loaded_count);
                    LOG_INFO(task_file_logger, "{} task: Loaded: {} new articles.", task_name, loaded_count);

                    loaded_count = 0;

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
                int loaded_count = 0;
                std::string repo_path;
                std::string error_filepath;
                std::shared_ptr<maddy::ParserConfig> maddy_config;
                std::shared_ptr<maddy::Parser> parser;
                utilities::cache::QuickStore<std::string, std::pair<task::types::PostAction, std::string>> cache;
                utilities::logging::LoggerFactory logger_factory;
                quill::Logger *task_logger;
                quill::Logger *task_file_logger;
        };

    }
}
