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

        class FindPostsTask : public Subscribable<std::string, std::pair<task::types::PostAction, std::string>> {
            public:
                FindPostsTask():
                    Subscribable<std::string, std::pair<task::types::PostAction, std::string>>(
                        "find_posts",
                        100,
                        1,
                        std::vector<std::string>{
                            "load_posts"
                        }
                    ),
                    posts_mapper(drogon::app().getDbClient())
                {

                }

                void initialize(Json::Value config){

                    task_logger = logger_factory.createConsoleLogger("console");
                    task_file_logger = logger_factory.createFileLogger("find_posts", "blog.jobs.log");

                    repo_path = config["repo_path"].asString();
                    auto env_repo_path = getenv("REPO_PATH");

                    if (repo_path.size() == 0 && !env_repo_path){

                        auto current_working_directory = std::filesystem::current_path().string();
                        std::stringstream repo_filepath;
                        repo_filepath << current_working_directory << "/posts";
                        repo_path = repo_filepath.str();
                        
                        auto repo_directory = std::filesystem::path(repo_path);

                        if (!std::filesystem::exists(repo_directory)){
                            std::filesystem::create_directory(repo_directory);
                        }

                    } else if (repo_path.size() == 0 && env_repo_path != NULL) {

                        repo_path = env_repo_path;

                    }

                    LOG_DEBUG(task_logger, "Initializing {} task. Please wait...", task_name);
                    LOG_INFO(task_file_logger, "Initializing {} task. Please wait...", task_name);

                };
                void run(){

                    std::string md_post_ext(".post.md");
                    std::string html_post_ext(".post.html");
                        
                    for (auto &file : std::filesystem::recursive_directory_iterator(repo_path))
                    {

                        auto filepath = file.path().string();

                        if (cache.get(filepath) == std::nullopt){

                            std::string file_type = file.path().extension();
                            std::string stub = file.path().stem().string();
                            std::string name = std::filesystem::path(stub).stem().string();

                            auto record_count = posts_mapper.countFuture(
                                drogon::orm::Criteria("name", drogon::orm::CompareOperator::EQ, name)
                            ).get();

                            auto filename = file.path().filename().string();


                            auto is_markdown = filename.find(md_post_ext) != std::string::npos;
                            auto is_html = filename.find(html_post_ext) != std::string::npos;
                            auto is_valid_post_document = is_html || is_markdown;

                            if (is_valid_post_document && record_count == 0){

                                send(
                                    filepath,
                                    std::pair(
                                        PostAction::LOAD_POST,
                                        filepath
                                    )
                                );


                                discovered_articles += 1;


                            }

                        }
                        

                    }

                    LOG_DEBUG(task_logger, "{} task: Discovered: {} new articles.", task_name, discovered_articles);
                    LOG_INFO(task_file_logger, "{} task: Discovered: {} new articles.", task_name, discovered_articles);

                    discovered_articles = 0;

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
                int discovered_articles = 0;
                std::string repo_path;
                
                drogon::orm::Mapper<drogon_model::sqlite3::Posts> posts_mapper;
                utilities::cache::QuickStore<std::string, std::pair<task::types::PostAction, std::string>> cache;
                utilities::logging::LoggerFactory logger_factory;
                quill::Logger *task_logger;
                quill::Logger *task_file_logger;
        };

    }
}
