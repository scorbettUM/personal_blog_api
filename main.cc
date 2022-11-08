#include <drogon/drogon.h>
#include <coroutine>
#include <chrono>
#include <unistd.h>
#include <cstdlib>
#include <./plugins/db/tasks/tasks.h>
#include <./plugins/global/cors/Cors.h>
#include <quill/Quill.h>


int main() {

    quill::Config config;
    quill::Handler* stdout_handler = quill::stdout_handler();
    stdout_handler->set_pattern("%(ascii_time) [%(process)] [%(thread)] %(logger_name) - %(message)", // format
                            "%Y-%m-%d %H:%M:%S.%Qms",  // timestamp format
                            quill::Timezone::GmtTime); 

    static_cast<quill::ConsoleHandler*>(stdout_handler)->enable_console_colours();

    config.default_handlers.emplace_back(stdout_handler);
    
    
    quill::configure(config);
    quill::start();

    auto logger = quill::get_logger();
    logger->set_log_level(quill::LogLevel::Info);
    

    LOG_DEBUG(logger, "Server running as process: {}", getpid());

    auto server_port = getenv("SERVER_PORT");
    if (server_port == NULL) {
        server_port = (char *)"8122";
    }

    
    drogon::app().addListener("0.0.0.0", std::stoi(server_port));
    //Load config file

    auto config_path = getenv("DROGON_CONFIG_PATH");
    if (config_path == NULL) {
        config_path = (char *)"../config.json";
    }


    drogon::app().loadConfigFile(config_path);

    LOG_INFO(logger,"Config successfully loaded!");

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

    drogon::app().run();
    return 0;
}
