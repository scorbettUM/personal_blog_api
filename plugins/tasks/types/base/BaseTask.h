#pragma once

#include <string>
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
                virtual void initialize(Json::Value config) = 0;
                virtual void run() = 0;
                virtual void stop() = 0;
                virtual void complete() = 0;
                virtual ~BaseTask(){};
                utilities::logging::LoggerFactory logger_factory;
                quill::Logger *logger;
                quill::Logger *file_logger;

            protected:
                BaseTask(
                    std::string name,
                    int id=0
                ){
                    task_name = name;
                    task_id = id;
                };

        };
    }
}

