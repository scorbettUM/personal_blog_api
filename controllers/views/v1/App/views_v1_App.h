#pragma once

#include <drogon/HttpController.h>
#include <string>

using namespace drogon;

namespace views
{
namespace v1
{
class App : public drogon::HttpController<App>
{
  public:
    METHOD_LIST_BEGIN
    // use METHOD_ADD to add your custom processing function here;
    // METHOD_ADD(App::get, "/{2}/{1}", Get); // path is /views/v1/App/{arg2}/{arg1}
    // METHOD_ADD(App::your_method_name, "/{1}/{2}/list", Get); // path is /views/v1/App/{arg1}/{arg2}/list
    // ADD_METHOD_TO(App::your_method_name, "/absolute/path/{1}/{2}/list", Get); // path is /absolute/path/{arg1}/{arg2}/list
    ADD_METHOD_TO(App::get_home, "", Get, Head, Options);
    ADD_METHOD_TO(App::get_post, "/{1}", Get, Head, Options);
    METHOD_LIST_END
    // your declaration of processing function maybe like this:
    // void get(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback, int p1, std::string p2);
    // void your_method_name(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback, double p1, int p2) const;
    Task<HttpResponsePtr> get_home(
      HttpRequestPtr req
    );
    Task<HttpResponsePtr> get_post(
      HttpRequestPtr req, 
      const std::string &post_name
    );


};
}
}
