/**
 *
 *  db_tasks_DatabaseManager.h
 *
 */

#pragma once
#include <drogon/plugins/Plugin.h>
#include <drogon/drogon.h>
#include <models/Posts.h>
#include <stdlib.h>
#include <cstdlib>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <stdio.h>
#include <time.h>
#include <cstdlib>
#include <utilities/filesystem/paths.h>

#ifndef LOGGER_FACTORY
#define LOGGER_FACTORY
#include <utilities/logging/logger_factory.h>
#endif





namespace db
{
namespace tasks
{

struct TableTypes {
  std::vector<std::pair<std::string, std::string>> tables = {

    std::pair(
      "Posts",
      "CREATE TABLE IF NOT EXISTS Posts(" \
      "post_id TEXT PRIMARY KEY NOT NULL," \
      "name TEXT UNIQUE NOT NULL," \
      "body TEXT UNIQUE NOT NULL," \
      "created DATETIME DEFAULT CURRENT_TIMESTAMP," \
      "updated DATETIME DEFAULT CURRENT_TIMESTAMP);"
    ),

    std::pair(
      "Tags",
      "CREATE TABLE IF NOT EXISTS Tags(" \
      "tag_id TEXT PRIMARY KEY NOT NULL," \
      "name TEXT NOT NULL," \
      "post TEXT NOT NULL," \
      "created DATETIME DEFAULT CURRENT_TIMESTAMP," \
      "updated DATETIME DEFAULT CURRENT_TIMESTAMP);" \
    ),

    std::pair(
      "Categories",
      "CREATE TABLE IF NOT EXISTS Categories(" \
      "category_id TEXT PRIMARY KEY NOT NULL," \
      "name TEXT NOT NULL," \
      "post TEXT NOT NULL," \
      "created DATETIME DEFAULT CURRENT_TIMESTAMP," \
      "updated DATETIME DEFAULT CURRENT_TIMESTAMP);" \
    ),

  }; 
};

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
