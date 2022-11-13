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

    task_group.addTask(PullUpdates());

    auto pull_task = PostTask();

    pull_task.subscribe(pull_task);

    task_group.addTask(pull_task);

    task_group.runStaggered();
}

void TaskManager::shutdown() 
{
    /// Shutdown the plugin
    task_group.stop();
}
