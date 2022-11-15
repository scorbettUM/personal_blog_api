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
class Tags : public drogon::HttpController<Tags>
{
  public:
    Tags(): drogon::HttpController<Tags>(){

      logger = logger_factory.createConsoleLogger("console");
      file_logger = logger_factory.createFileLogger("api_v1_posts", "blog.api.log");

    }
    METHOD_LIST_BEGIN
    // use METHOD_ADD to add your custom processing function here;
    // METHOD_ADD(Tags::get, "/{2}/{1}", Get); // path is /api/v1/Tags/{arg2}/{arg1}
    // METHOD_ADD(Tags::your_method_name, "/{1}/{2}/list", Get); // path is /api/v1/Tags/{arg1}/{arg2}/list
    // ADD_METHOD_TO(Tags::your_method_name, "/absolute/path/{1}/{2}/list", Get); // path is /absolute/path/{arg1}/{arg2}/list
    METHOD_ADD(Tags::list, "/list", Get, Head, Options);
    METHOD_LIST_END
    // your declaration of processing function maybe like this:
    // void get(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback, int p1, std::string p2);
    // void your_method_name(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback, double p1, int p2) const;
    Task<HttpResponsePtr> list(
      HttpRequestPtr req
    );

  private:
    utilities::logging::LoggerFactory logger_factory;
    quill::Logger* logger;
    quill::Logger* file_logger;

};
}
}
