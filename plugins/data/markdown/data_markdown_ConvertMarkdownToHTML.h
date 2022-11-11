/**
 *
 *  data_markdown_ConvertMarkdownToHTML.h
 *
 */

#pragma once

#include <drogon/plugins/Plugin.h>
#include <drogon/drogon.h>
#include <models/Posts.h>
#include <models/Tags.h>
#include <models/Categories.h>
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
#include "uuid.h"
#include <utilities/filesystem/paths.h>
#include <utilities/string/string_utils.h>
#include <utilities/storage/quick_store.h>


#include "parser/parser.h"


#ifndef LOGGER_FACTORY
#define LOGGER_FACTORY
#include <utilities/logging/logger_factory.h>
#endif



namespace data
{
namespace markdown
{

enum CacheAction {
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

class ConvertMarkdownToHTML : public drogon::Plugin<ConvertMarkdownToHTML>
{
  public:
    ConvertMarkdownToHTML(): 
      db(),
      posts_mapper(db),
      tags_mapper(db),
      categories_mapper(db),
      maddy_config(std::make_shared<maddy::ParserConfig>()),
      parser(std::make_shared<maddy::Parser>(maddy_config))
    {

    }
    /// This method must be called by drogon to initialize and start the plugin.
    /// It must be implemented by the user.
    void initAndStart(const Json::Value &config) override;
    /// This method must be called by drogon to shutdown the plugin.
    /// It must be implemented by the user.
    void shutdown() override;

  private:
    void findPosts(const std::string &repo_path);
    void loadPosts();
    void savePosts();
    void findTagsAndCategories();
    void loadTagsAndCategories();
    void saveTags();
    void saveCategories();
    std::string repo_path;
    std::thread markdown_job;
    std::atomic_bool run_job = false;
    std::condition_variable runner_conditional;
    std::mutex runner_mutex;
    drogon::orm::DbClientPtr db;
    drogon::orm::Mapper<drogon_model::sqlite3::Posts> posts_mapper;
    drogon::orm::Mapper<drogon_model::sqlite3::Tags> tags_mapper;
    drogon::orm::Mapper<drogon_model::sqlite3::Categories> categories_mapper;
    std::shared_ptr<maddy::ParserConfig> maddy_config;
    std::shared_ptr<maddy::Parser> parser;

    utilities::logging::LoggerFactory logger_factory;
    quill::Logger *logger;
    quill::Logger *file_logger;
    utilities::cache::QuickStore<std::string, std::pair<data::markdown::CacheAction, std::string>> cache;
};

}
}
