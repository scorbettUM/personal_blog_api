/**
 *
 *  task_TaskManager.h
 *
 */

#pragma once

#include <drogon/plugins/Plugin.h>
#include "types/post/FindPostsTask.h"
#include "types/post/LoadPostsTask.h"
#include "types/post/SavePostsTask.h"
#include "types/post/FindMetadataTask.h"
#include "types/post/LoadMetadataTask.h"
#include "types/post/SaveTagsTask.h"
#include "types/post/SaveCategoriesTask.h"
#include "types/repo/PullUpdates.h"
#include "TaskGroup.h"


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
    task::TaskGroup<task::types::BaseTask> task_group;
};

}
