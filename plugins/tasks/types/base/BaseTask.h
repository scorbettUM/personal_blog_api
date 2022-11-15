#pragma once

#include <string>
#include <vector>
#include <drogon/drogon.h>

#ifndef LOGGER_FACTORY
#define LOGGER_FACTORY
#include <utilities/logging/logger_factory.h>
#endif



namespace task {
    namespace types {
        class BaseTask {
            public:
                std::string task_name;
                int task_id;
                int task_order;
                std::vector<std::string> task_dependents;
                virtual void initialize(Json::Value config) = 0;
                virtual void run() = 0;
                virtual void stop() = 0;
                virtual void complete() = 0;
                virtual ~BaseTask(){};

            protected:
                BaseTask(
                    std::string name,
                    int id=0,
                    int order=0,
                    std::vector<std::string> dependents=std::vector<std::string>()
                ){
                    task_name = name;
                    task_id = id;
                    task_order = order;
                    task_dependents = dependents;
                };

        };
    }
}

