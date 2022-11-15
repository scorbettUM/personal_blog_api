#include <map>
#include "types/post/FindPostsTask.h"
#include "types/post/LoadPostsTask.h"
#include "types/post/SavePostsTask.h"
#include "types/post/FindMetadataTask.h"
#include "types/post/LoadMetadataTask.h"
#include "types/post/SaveTagsTask.h"
#include "types/post/SaveCategoriesTask.h"
#include "types/repo/PullUpdates.h"

#ifndef POSTS_COMMON
#define POSTS_COMMON
#include <plugins/tasks/types/base/BaseTask.h>
#endif

using namespace task::types;

namespace task {
    template<typename RegistryType>
    class TaskRegistry {
        public:
            template<typename... Tasks> TaskRegistry(Tasks... tasks){


                std::vector<std::shared_ptr<RegistryType>> task_types = {tasks...};
                for (auto &task_type : task_types){
                    registry[task_type->task_name] = task_type;
                }

                sorted_registry = task_types;

                std::sort(
                    sorted_registry.begin(),
                    sorted_registry.end(),
                    [&](std::shared_ptr<RegistryType> task_a, std::shared_ptr<RegistryType> task_b){
                        return task_a->task_order < task_b->task_order;
                    }
                );

            }

            std::vector<std::shared_ptr<RegistryType>> iter(){
                return sorted_registry;
            }

            std::shared_ptr<RegistryType> get(std::string task_name){
                return registry[task_name];
            }


        private:
            std::map<std::string, std::shared_ptr<RegistryType>> registry;
            std::vector<std::shared_ptr<RegistryType>> sorted_registry;
    };
}