/**
 *
 *  app_global_Cors.h
 *
 */

#pragma once
#include <drogon/plugins/Plugin.h>
#include "CorsOptions.h"
#include <vector>
#include <vector>
#include <iterator>
#include <sstream>
#include <string>
#include <drogon/drogon.h>

#ifndef LOGGER_FACTORY
#define LOGGER_FACTORY
#include <utilities/logging/logger_factory.h>
#endif

namespace app
{
namespace global
{

class Cors : public drogon::Plugin<Cors>
{
  public:
    Cors() {}
    /// This method must be called by drogon to initialize and start the plugin.
    /// It must be implemented by the user.
    void initAndStart(const Json::Value &config) override;
    void setHeaders(const std::vector<CORSOption> &options, const HttpResponsePtr &response) const;
    /// This method must be called by drogon to shutdown the plugin.
    /// It must be implemented by the user.
    void shutdown() override;
};

}
}
