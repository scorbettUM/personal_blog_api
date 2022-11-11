#include <drogon/drogon.h>


#ifndef LOGGER_FACTORY
#define LOGGER_FACTORY
#include <utilities/logging/logger_factory.h>
#endif


#include <coroutine>
#include <chrono>
#include <unistd.h>
#include <cstdlib>
#include <filesystem>
#include <plugins/global/cors/Cors.h>

#ifndef CACHE
#define CACHE
#include "utilities/storage/quick_store.h"
#endif


int main() {
    
    
    quill::start();

    auto logger_factory = utilities::logging::LoggerFactory();

    auto current_working_directory = std::filesystem::current_path().string();
    
    std::stringstream config_filepath;
    config_filepath << current_working_directory << "/config.json";

    auto logger = logger_factory.createConsoleLogger("console");
    auto file_logger = logger_factory.createFileLogger("main", "blog.main.log");

    LOG_DEBUG(logger, "Server running as process: {}", getpid());
    LOG_INFO(file_logger, "Server running as process: {}", getpid());

    auto server_port = getenv("SERVER_PORT");
    if (server_port == NULL) {
        server_port = (char *)"8122";
    }
    
    drogon::app().addListener("0.0.0.0", std::stoi(server_port));

    //Load config file
    std::string api_config_path = config_filepath.str();
    auto config_path = getenv("DROGON_CONFIG_PATH");
    if (config_path) {
        api_config_path = std::string(config_path);
    }


    drogon::app().loadConfigFile(api_config_path);

    LOG_INFO(logger,"Config successfully loaded from: {}", api_config_path);
    LOG_INFO(file_logger,"Config successfully loaded from: {}", api_config_path);

    const std::vector<CORSOption> options {
        CORSOption(
            std::string("Access-Control-Allow-Origin"),
            std::vector<std::string>{"*"}
        ),
        CORSOption(
            std::string("Access-Control-Request-Method"),
            std::vector<std::string>{
                "GET",
                "HEAD",
                "OPTIONS"
            }
        )
    };

    drogon::app().registerHttpResponseCreationAdvice([&](
        const HttpResponsePtr &response
    ){

        auto cors_plugin = drogon::app().getPlugin<app::global::Cors>();
        cors_plugin->setHeaders(options, response);
        
    });

    LOG_INFO(logger, "Serving on port: {}\n", server_port);
    LOG_INFO(file_logger, "Serving on port: {}\n", server_port);

    drogon::app().run();
    return 0;
}
