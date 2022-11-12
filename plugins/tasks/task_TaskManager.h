/**
 *
 *  task_TaskManager.h
 *
 */

#pragma once

#include <drogon/plugins/Plugin.h>
#include "types/post/PostTask.h"
#include "types/repo/PullUpdates.h"
#include "TaskGroup.h"

#ifndef BASE_TASK
#define BASE_TASK
#include <plugins/tasks/types/base/Task.h>
#endif

namespace task
{

class TaskManager : public drogon::Plugin<TaskManager>
{
  public:
    TaskManager() {}
    /// This method must be called by drogon to initialize and start the plugin.
    /// It must be implemented by the user.
    void initAndStart(const Json::Value &config) override;

    /// This method must be called by drogon to shutdown the plugin.
    /// It must be implemented by the user.
    void shutdown() override;

  private:
    task::TaskGroup task_group;
};

}
