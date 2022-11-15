#pragma once

#include <string>
#include <cstdlib>
#include <atomic>
#include <iostream>
#include <condition_variable>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <stdlib.h>
#include <map>
#include <tuple>
#include <drogon/drogon.h>
#include <models/Posts.h>
#include <models/Tags.h>
#include <models/Categories.h>

#ifndef LOGGER_FACTORY
#define LOGGER_FACTORY
#include <utilities/logging/logger_factory.h>
#endif

#ifndef PUBSUB
#define PUBSUB
#include <plugins/tasks/types/base/BaseTask.h>
#endif

#ifndef GIT
#define GIT
#include "git/git.h"
#endif


namespace task {
    namespace types {

        class PullUpdates :  public BaseTask {
            public:

                PullUpdates(): BaseTask("pull_updates") {

                }

                void initialize(Json::Value config){

                    logger = logger_factory.createConsoleLogger("console");
                    file_logger = logger_factory.createFileLogger("articles_job", "blog.articles.job.log");

                    auto repo_url = config["repo_url"].asString();
                    if (repo_url.size() == 0){

                        auto config_url_env = getenv("REPO_URL");
                        if (config_url_env == NULL){
                            throw std::runtime_error("Err. - You must specify a repo url!");   
                        } else {
                            repo_url = std::string((char*)config_url_env);
                        }

                    }

                    auto repo_path = config["repo_path"].asString();
                    auto env_repo_path = getenv("REPO_PATH");

                    if (repo_path.size() == 0 && env_repo_path == NULL){

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

                    auto repo_branch = config["repo_branch"].asString();
                    if (repo_branch.size() == 0){

                        auto config_branch_env = getenv("REPO_BRANCH");
                        if (config_branch_env == NULL){
                            repo_branch = "main";   
                        } else {
                            repo_branch = std::string((char*)config_branch_env);
                        }

                    }

                    auto repo_remote = config["repo_remote"].asString();
                    if (repo_remote.size() ==  0){

                        auto config_remote_env = getenv("REPO_REMOTE");
                        if (config_remote_env == NULL){
                            repo_remote = "origin";
                            
                        } else {
                            repo_remote = std::string((char*)config_remote_env);
                        }

                    }

                    LOG_DEBUG(logger, "Git -> Articles update job: Targeting repository url {}", repo_url);
                    LOG_DEBUG(logger, "Git -> Articles update job: Targeting repository remote {}", repo_remote);
                    LOG_DEBUG(logger, "Git -> Articles update job: Targeting repository branch {}", repo_branch);
                    LOG_DEBUG(logger, "Git -> Articles update job: Targeting repository path {}", repo_path);


                    LOG_INFO(file_logger, "Git -> Articles update job: Targeting repository url {}", repo_url);
                    LOG_INFO(file_logger, "Git -> Articles update job: Targeting repository remote {}", repo_remote);
                    LOG_INFO(file_logger, "Git -> Articles update job: Targeting repository branch {}", repo_branch);
                    LOG_INFO(file_logger, "Git -> Articles update job: Targeting repository path {}", repo_path);

                    repo_config = RepoConfig(
                        repo_remote,
                        repo_path,
                        repo_branch,
                        repo_url
                    );

                    git = Git(repo_config);

                    git.clone();
                    git.fetch();

                    if (repo_branch != "main"){
                        git.checkout();
                    }



                    LOG_INFO(logger, "Started Git -> Articles update task.");
                    LOG_INFO(file_logger, "Started Git -> Articles update task.");

                };

      
                void run() {

                    LOG_DEBUG(logger, "Git -> Articles update task: Running on process: {}", getpid());
                    LOG_INFO(file_logger, "Git -> Articles update task: Running on process: {}", getpid());


                    try {

                        git.pull();
                        
                    } catch(std::exception &e){
                        LOG_CRITICAL(logger, "Git -> Articles update task: Encountered critical error: {}. Restarting.", e.what());
                        LOG_CRITICAL(file_logger, "Git -> Articles update task: Encountered critical error: {}. Restarting.", e.what());
                    }
                    
                };

                void stop() {};
                void complete() {};

            private:
                RepoConfig repo_config;
                Git git;
        };

    }
}