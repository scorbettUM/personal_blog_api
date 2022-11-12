/**
 *
 *  task_TaskManager.cc
 *
 */

#include "task_TaskManager.h"

using namespace drogon;
using namespace task;
using namespace task::types;

void TaskManager::initAndStart(const Json::Value &config)
{
    /// Initialize and start the plugin

    task_group.parseConfig(config);

    task_group.addTask(
        std::make_shared<PullUpdates>(PullUpdates())
    );

    task_group.addTask(
        std::make_shared<PostTask>(PostTask())
    );

    task_group.runStaggered();
}

void TaskManager::shutdown() 
{
    /// Shutdown the plugin
    task_group.stop();
}
