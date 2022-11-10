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
#include "uuid.h"
#include <utilities/filesystem/paths.h>
#include <utilities/string/string_utils.h>


#include "parser/parser.h"
#include "../articles/git/repo_config.h"


#ifndef LOGGER_FACTORY
#define LOGGER_FACTORY
#include <utilities/logging/logger_factory.h>
#endif



namespace data
{
namespace markdown
{

struct RawPost {
  RawPost(): name(), data(), tags(std::vector<std::string>()), categories(std::vector<std::string>()) {}
  RawPost(
    std::string post_name,
    std::string post_data
  ): name(post_name), data(post_data), tags(std::vector<std::string>()), categories(std::vector<std::string>()) {}
  std::string name;
  std::string data;
  std::vector<std::string> tags;
  std::vector<std::string> categories;
};

class ConvertMarkdownToHTML : public drogon::Plugin<ConvertMarkdownToHTML>
{
  public:
    ConvertMarkdownToHTML(): repo_config(manager::RepoConfig()) {}
    manager::RepoConfig repo_config;
    std::thread markdown_job;
    std::atomic_bool run_job = false;
    std::condition_variable runner_conditional;
    std::mutex runner_mutex;
    /// This method must be called by drogon to initialize and start the plugin.
    /// It must be implemented by the user.
    void initAndStart(const Json::Value &config) override;
    /// This method must be called by drogon to shutdown the plugin.
    /// It must be implemented by the user.
    void shutdown() override;
};

}
}
