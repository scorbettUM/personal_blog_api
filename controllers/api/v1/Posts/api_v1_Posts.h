#pragma once

#include <drogon/HttpController.h>

#include <vector>
#include <string>

#ifndef LOGGER_FACTORY
#define LOGGER_FACTORY
#include <utilities/logging/logger_factory.h>
#endif


using namespace drogon;

namespace api
{
namespace v1
{
class Posts : public drogon::HttpController<Posts>
{
  public:
    Posts(): drogon::HttpController<Posts>(){

      logger = logger_factory.createConsoleLogger("console");
      file_logger = logger_factory.createFileLogger("api_v1_posts", "blog.api.log");

    }
    METHOD_LIST_BEGIN
    // use METHOD_ADD to add your custom processing function here;
    // METHOD_ADD(Posts::get, "/{2}/{1}", Get); // path is /api/v1/Posts/{arg2}/{arg1}
    // METHOD_ADD(Posts::your_method_name, "/{1}/{2}/list", Get); // path is /api/v1/Posts/{arg1}/{arg2}/list
    // ADD_METHOD_TO(Posts::your_method_name, "/absolute/path/{1}/{2}/list", Get); // path is /absolute/path/{arg1}/{arg2}/list

    METHOD_ADD(Posts::get, "/{1}", Get, Head, Options);
    METHOD_ADD(Posts::getByTags, "/tags/{1}", Get, Head, Options);
    METHOD_ADD(Posts::getByCategories, "/categories/{1}", Get, Head, Options);
    METHOD_ADD(Posts::list, "/list", Get, Head, Options);
    METHOD_ADD(Posts::listTitles, "/titles", Get, Head, Options);
    METHOD_LIST_END
    // your declaration of processing function maybe like this:
    // void get(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback, int p1, std::string p2);
    // void your_method_name(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback, double p1, int p2) const;
  Task<HttpResponsePtr> get(
    HttpRequestPtr req, 
    std::string post_title
  );

  Task<HttpResponsePtr> getByTags(
    HttpRequestPtr req,
    std::string tags
  );

  Task<HttpResponsePtr> getByCategories(
    HttpRequestPtr req,
    std::string categories
  );
  
  Task<HttpResponsePtr> list(
    HttpRequestPtr req
  );

  Task<HttpResponsePtr> listTitles(
    HttpRequestPtr req
  );

  private:
    utilities::logging::LoggerFactory logger_factory;
    quill::Logger* logger;
    quill::Logger* file_logger;

};
}
}
