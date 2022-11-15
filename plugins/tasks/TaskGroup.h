#pragma once

#include <atomic>
#include <string>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>
#include <quill/Quill.h>

#ifndef POSTS_COMMON
#define POSTS_COMMON
#include "types/PostAction.h"
#include <plugins/tasks/types/base/Subscribable.h>
#endif



namespace task {

    template<typename GroupType>
    class TaskGroup {

        public:
            TaskGroup(
            )
            {

                logger = logger_factory.createConsoleLogger("console");
                file_logger = logger_factory.createFileLogger("markdown_job", "blog.posts.job.log");

            }

            void parseConfig(Json::Value tasks_config){
                config = tasks_config;
                auto process_interval = getenv("PROCESS_INTERVAL");
                if (process_interval != NULL){
                    task_process_interval = std::stoi((char*)process_interval);

                } else if(!config["process_interval"].empty()){
                    task_process_interval = config["process_interval"].asInt();
                }

            }

            template<typename T> void addTask(T task){

                if (config[task->task_name].empty()){
                    config[task->task_name] = Json::Value();
                }

                task->task_id = task_idx;
                tasks.push_back(task);

                task_idx += 1;
            }


            void runStaggered(){

                run_tasks = true;
                
                for (auto &group_task : tasks){
                    
                    auto task_thread = std::thread([&](){
                        
                        int launch_interval = task_process_interval/tasks.size();

                        std::unique_lock<std::mutex> lock(runner_mutex);
                        runner_conditional.wait_for(
                            lock, 
                            std::chrono::duration(
                                std::chrono::seconds(launch_interval * group_task->task_id)
                            ),  
                            [&](){
                                return !run_tasks;
                            }
                            );

                        auto task_config = config[group_task->task_name];

                        if (task_config.empty()){
                            task_config = Json::Value(Json::objectValue);
                        }
                        group_task->initialize(task_config);

        

                        while (run_tasks){

                            group_task->run();
                

                            runner_conditional.wait_for(
                                lock, 
                                std::chrono::duration(
                                    std::chrono::seconds(task_process_interval)
                                ),  
                                [&](){
                                    return !run_tasks;
                                }
                            );

                        }

                        
                        group_task->stop();
                        group_task->complete();

                    });

                    threads.push_back(
                        std::move(task_thread)
                    );
                }
                
            }

            void stop(){
                

                LOG_INFO(logger, "Task Group has been notified of shutdown. Please wait...");
                LOG_INFO(file_logger, "Task Group has been notified of shutdown. Please wait...");

                run_tasks = false;
                runner_conditional.notify_all();
                for (std::thread &thread : threads){

                    if (thread.joinable()){
                        thread.join();
                    }

                }

                threads.erase(threads.begin(), threads.end());
                tasks.erase(tasks.begin(), tasks.end());

                LOG_INFO(logger, "Task Group has completed.");
                LOG_INFO(file_logger, "Task Group has completed.");
                
            }

        private:
            int task_process_interval = 15;
            int task_idx = 0;
            Json::Value config;
            std::atomic_bool run_tasks;
            std::condition_variable runner_conditional;
            std::mutex runner_mutex;
            std::vector<std::shared_ptr<GroupType>> tasks;
            std::vector<std::thread> threads;
            utilities::logging::LoggerFactory logger_factory;
            quill::Logger *logger;
            quill::Logger *file_logger;
    };
}