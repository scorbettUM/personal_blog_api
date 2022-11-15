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

    auto registry = TaskRegistry<Subscribable<std::string, std::pair<task::types::PostAction, std::string>>>(
        std::make_shared<FindPostsTask>(FindPostsTask()),
        std::make_shared<LoadPostsTask>(LoadPostsTask()),
        std::make_shared<SavePostsTask>(SavePostsTask()),
        std::make_shared<FindMetadataTask>(FindMetadataTask()),
        std::make_shared<LoadMetadataTask>(LoadMetadataTask()),
        std::make_shared<SaveTagsTask>(SaveTagsTask()),
        std::make_shared<SaveCategoriesTask>(SaveCategoriesTask())
    );

    for (auto &task: registry.iter()){
        for (auto &dependent : task->task_dependents){
            task->subscribe(registry.get(dependent));
        }

        task_group.addTask(std::move(task));

    }

    auto pull_updates = std::make_shared<PullUpdates>(PullUpdates());
    task_group.addTask(pull_updates);

    task_group.runStaggered();
}

void TaskManager::shutdown() 
{
    /// Shutdown the plugin
    task_group.stop();
}
