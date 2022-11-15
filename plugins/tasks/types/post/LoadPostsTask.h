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
                    Subscribable(
                        "load_posts"
                    ),
                    maddy_config(std::make_shared<maddy::ParserConfig>()),
                    parser(std::make_shared<maddy::Parser>(maddy_config))
                {

                }

                void initialize(Json::Value config){

                    logger = logger_factory.createConsoleLogger("console");
                    file_logger = logger_factory.createFileLogger("markdown_job", "blog.posts.job.log");

                    maddy_config = std::make_shared<maddy::ParserConfig>();
                    maddy_config->isEmphasizedParserEnabled = true; // default
                    maddy_config->isHTMLWrappedInParagraph = true; // default

                    parser = std::make_shared<maddy::Parser>(maddy_config);

                };
                void run(){

                    int cache_idx = 0;
                    std::vector<std::string> processed;

                    for (const auto &filepath: cache.iter()){

                        auto cache_item = cache.get(filepath).value();

                        if (cache_item.first == PostAction::LOAD_POST){
                            std::stringstream buffer;
                            std::ifstream ifs(filepath);
                            std::string post_body;

                            auto path = std::filesystem::path(filepath);
                            std::string file_type = path.extension();

                            if ( !ifs.is_open() ) { 
                                LOG_WARNING(logger, "Markdown -> HTML converter task: Failed to open article at: {}", filepath);  
                                LOG_WARNING(file_logger, "Markdown -> HTML converter task: Failed to open article at: {}", filepath);   

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


                    LOG_DEBUG(logger, "Markdown -> HTML converter task: Loaded: {} new articles.", loaded_count);
                    LOG_INFO(file_logger, "Markdown -> HTML converter task: Loaded: {} new articles.", loaded_count);

                    loaded_count = 0;

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
                int loaded_count = 0;
                std::string repo_path;
                std::shared_ptr<maddy::ParserConfig> maddy_config;
                std::shared_ptr<maddy::Parser> parser;
                utilities::cache::QuickStore<std::string, std::pair<task::types::PostAction, std::string>> cache;
        };

    }
}
