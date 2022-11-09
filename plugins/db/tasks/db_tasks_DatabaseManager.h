/**
 *
 *  db_tasks_DatabaseManager.h
 *
 */

#pragma once
#include <drogon/drogon.h>
#include <drogon/plugins/Plugin.h>
#include <models/Posts.h>
#include <stdlib.h>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <time.h>
#include <cstdlib>

#ifndef LOGGER_FACTORY
#define LOGGER_FACTORY
#include <utilities/logging/logger_factory.h>
#endif


namespace db
{
namespace tasks
{

class DatabaseManager : public drogon::Plugin<DatabaseManager>
{
  public:
    DatabaseManager() {}
    /// This method must be called by drogon to initialize and start the plugin.
    /// It must be implemented by the user.
    void initAndStart(const Json::Value &config) override;

    /// This method must be called by drogon to shutdown the plugin.
    /// It must be implemented by the user.
    void shutdown() override;
};

}
}
