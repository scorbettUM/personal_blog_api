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

    auto find_posts = std::make_shared<FindPostsTask>(FindPostsTask());
    auto load_posts = std::make_shared<LoadPostsTask>(LoadPostsTask());
    auto save_posts = std::make_shared<SavePostsTask>(SavePostsTask());
    auto find_metadata = std::make_shared<FindMetadataTask>(FindMetadataTask());
    auto load_metadata = std::make_shared<LoadMetadataTask>(LoadMetadataTask());
    auto save_tags = std::make_shared<SaveTagsTask>(SaveTagsTask());
    auto save_categories = std::make_shared<SaveCategoriesTask>(SaveCategoriesTask());
    auto pull_updates = std::make_shared<PullUpdates>(PullUpdates());

    tasks.push_back(find_posts);
    tasks.push_back(load_posts);
    tasks.push_back(save_posts);
    tasks.push_back(find_metadata);
    tasks.push_back(load_metadata);
    tasks.push_back(save_tags);
    tasks.push_back(save_categories);

    find_posts->subscribe(load_posts);
    load_posts->subscribe(save_posts);
    save_posts->subscribe(find_metadata);
    find_metadata->subscribe(load_metadata);
    load_metadata->subscribe(save_tags);
    load_metadata->subscribe(save_categories);

    task_group.parseConfig(config);

    task_group.addTask(pull_updates);
    task_group.addTask(find_posts);
    task_group.addTask(load_posts);
    task_group.addTask(save_posts);
    task_group.addTask(find_metadata);
    task_group.addTask(load_metadata);
    task_group.addTask(save_tags);
    task_group.addTask(save_categories);

    task_group.runStaggered();
}

void TaskManager::shutdown() 
{
    /// Shutdown the plugin
    task_group.stop();
}
