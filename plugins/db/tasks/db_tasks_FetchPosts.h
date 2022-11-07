/**
 *
 *  db_tasks_FetchPosts.h
 *
 */

#pragma once

#include <drogon/plugins/Plugin.h>
namespace db
{
namespace tasks
{

class FetchPosts : public drogon::Plugin<FetchPosts>
{
  public:
    FetchPosts() {}
    /// This method must be called by drogon to initialize and start the plugin.
    /// It must be implemented by the user.
    void initAndStart(const Json::Value &config) override;

    /// This method must be called by drogon to shutdown the plugin.
    /// It must be implemented by the user.
    void shutdown() override;
};

}
}
