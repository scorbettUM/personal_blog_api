/**
 *
 *  app_global_Cors.cc
 *
 */

#include "app_global_Cors.h"


using namespace drogon;
using namespace app::global;

void Cors::initAndStart(const Json::Value &config)
{   
    (void)config;
}


void Cors::setHeaders(
    const std::vector<CORSOption> &options,
    const HttpResponsePtr &response
) const {

    auto logger_factory = utilities::logging::LoggerFactory();
    auto logger = logger_factory.createConsoleLogger("console");
    auto file_logger = logger_factory.createFileLogger("main", "blog.main.log");

    LOG_INFO(logger, "Cors: Setting default CORS header options.");
    LOG_INFO(file_logger, "Cors: Setting default CORS header options.");

    const auto cors_options = CORSOptions(options);
    cors_options.buildHeaders(response);

}


void Cors::shutdown() 
{
    /// Shutdown the plugin
}
