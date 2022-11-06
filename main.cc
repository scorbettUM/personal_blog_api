#include <drogon/drogon.h>
#include <coroutine>
#include <chrono>
#include <./plugins/db/tasks/tasks.h>
#include <./plugins/global/cors/Cors.h>


int main() {
    //Set HTTP listener address and port
    drogon::app().addListener("0.0.0.0",8122);
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


    drogon::app().getLoop()->runEvery(
        std::chrono::seconds(2),
        [](){
            
            auto posts_plugin = drogon::app().getPlugin<db::tasks::FetchPosts>();

            posts_plugin->getNewPosts();
        }
    );
    //Run HTTP framework,the method will block in the internal event loop
    drogon::app().run();
    return 0;
}
