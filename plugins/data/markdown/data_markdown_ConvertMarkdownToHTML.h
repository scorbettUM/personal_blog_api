/**
 *
 *  data_markdown_ConvertMarkdownToHTML.h
 *
 */

#pragma once

#include <drogon/plugins/Plugin.h>
#include "parser/parser.h"
#include "../articles/git/repo_config.h"
#include <string>
#include <cstdlib>
#include <atomic>
#include <iostream>
#include <condition_variable>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <stdlib.h>
#include "uuid.h"

namespace data
{
namespace markdown
{

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
