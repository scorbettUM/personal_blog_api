/**
 *
 *  data_articles_ArticlesManager.h
 *
 */

#pragma once

#include <drogon/plugins/Plugin.h>
#include <drogon/drogon.h>
#include "git/git.h"
#include <iostream>

namespace data
{
namespace articles
{

class ArticlesManager : public drogon::Plugin<ArticlesManager>
{
  public:
    ArticlesManager() {}
    /// This method must be called by drogon to initialize and start the plugin.
    /// It must be implemented by the user.
    void initAndStart(const Json::Value &config) override;

    void updateArticles() const;

    /// This method must be called by drogon to shutdown the plugin.
    /// It must be implemented by the user.
    void shutdown() override;
};

}
}
