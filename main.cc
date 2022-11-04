#include <drogon/drogon.h>
#include <coroutine>
#include <chrono>
#include <./plugins/db_tasks_FetchPosts.h>


int main() {
    //Set HTTP listener address and port
    drogon::app().addListener("0.0.0.0",8122);
    //Load config file
    drogon::app().loadConfigFile("../config.json");

    drogon::app().getLoop()->runEvery(
        std::chrono::minutes(15),
        [](){
            auto posts_plugin = drogon::app().getPlugin<db::tasks::FetchPosts>();

            posts_plugin->getNewPosts();
        }
    );
    //Run HTTP framework,the method will block in the internal event loop
    drogon::app().run();
    return 0;
}
