/**
 *
 *  data_articles_ArticlesManager.h
 *
 */

#pragma once

#include <drogon/plugins/Plugin.h>
#include <drogon/drogon.h>
#include "git/git.h"
#include <atomic>
#include <iostream>
#include <condition_variable>
#include <mutex>


#ifndef LOGGER_FACTORY
#define LOGGER_FACTORY
#include <utilities/logging/logger_factory.h>
#endif


namespace data
{
namespace articles
{

class ArticlesManager : public drogon::Plugin<ArticlesManager>
{
  public:
    ArticlesManager(): git_config(manager::RepoConfig()), runner(manager::Git()){

    }
    manager::RepoConfig git_config;
    manager::Git runner;
    std::thread runner_job;
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
