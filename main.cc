#include <drogon/drogon.h>
#include <coroutine>
#include <chrono>
#include <unistd.h>
#include <./plugins/db/tasks/tasks.h>
#include <./plugins/global/cors/Cors.h>


int main() {

    auto server_port = getenv("BLOG_SERVER_PORT");
    if (server_port == NULL){
        server_port = (char*)"8122";
    }
    
    //Set HTTP listener address and port
    std::cout<<"Server running as process: "<<getpid()<<std::endl;
    drogon::app().addListener("0.0.0.0", std::stoi(server_port));
    //Load config file

    auto config_path = std::string("../config.json");

    drogon::app().loadConfigFile(config_path);

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


    std::cout<<"Serving on port: "<<server_port<<"\n"<<std::endl;

    drogon::app().run();
    return 0;
}
