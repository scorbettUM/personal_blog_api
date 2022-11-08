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


struct timer_killer {
  // returns false if killed:
  template<class R, class P>
  bool wait_for( std::chrono::duration<R,P> const& time ) const {
    std::unique_lock<std::mutex> lock(m);
    return !cv.wait_for(lock, time, [&]{return terminate;});
  }
  void kill() {
    std::unique_lock<std::mutex> lock(m);
    terminate=true; // should be modified inside mutex lock
    cv.notify_all(); // it is safe, and *sometimes* optimal, to do this outside the lock
  }
  // I like to explicitly delete/default special member functions:
  timer_killer() = default;
  timer_killer(timer_killer&&)=delete;
  timer_killer(timer_killer const&)=delete;
  timer_killer& operator=(timer_killer&&)=delete;
  timer_killer& operator=(timer_killer const&)=delete;
private:
  mutable std::condition_variable cv;
  mutable std::mutex m;
  bool terminate = false;
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
