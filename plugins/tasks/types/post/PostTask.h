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
#include <utilities/uuid/uuid.h>
#include <utilities/filesystem/paths.h>
#include <utilities/string/string_utils.h>
#include <utilities/storage/quick_store.h>


#include "parser/parser.h"


#ifndef BASE_TASK
#define BASE_TASK
#include <plugins/tasks/types/base/BaseTask.h>
#endif




namespace task {

    namespace types {

        enum PostAction {
            DISCOVER_POST,
            DISCOVER_METADATA,
            LOAD_POST,
            LOAD_METADATA,
            SAVE_POST,
            SAVE_TAGS,
            SAVE_CATEGORIES,
            PROCESSED_POST,
            PROCESSED_TAGS,
            PROCESSED_CATEGORIES,
        };

        enum MetadataType {
            TAGS,
            CATEGORIES,
        };

        class PostTask : public BaseTask  {
            public:
                PostTask():
                    BaseTask(
                        "markdown_to_html",
                        0
                    ),
                    db(),
                    posts_mapper(db),
                    tags_mapper(db),
                    categories_mapper(db),
                    maddy_config(std::make_shared<maddy::ParserConfig>()),
                    parser(std::make_shared<maddy::Parser>(maddy_config))
                {

                }

                void initialize(Json::Value config) override;
                void run() override;
                /// This method must be called by drogon to shutdown the plugin.
                /// It must be implemented by the user.
                void stop() override;
                void complete() override;

            private:
                int article_process_interval = 60;
                void findPosts(const std::string &repo_path);
                void loadPosts();
                void savePosts();
                void findTagsAndCategories();
                void loadTagsAndCategories();
                void saveTags();
                void saveCategories();
                std::string repo_path;
                drogon::orm::DbClientPtr db;
                drogon::orm::Mapper<drogon_model::sqlite3::Posts> posts_mapper;
                drogon::orm::Mapper<drogon_model::sqlite3::Tags> tags_mapper;
                drogon::orm::Mapper<drogon_model::sqlite3::Categories> categories_mapper;
                std::shared_ptr<maddy::ParserConfig> maddy_config;
                std::shared_ptr<maddy::Parser> parser;
                utilities::cache::QuickStore<std::string, std::pair<task::types::PostAction, std::string>> cache;
        };

    }
}
