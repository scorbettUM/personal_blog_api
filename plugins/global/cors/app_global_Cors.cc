/**
 *
 *  app_global_Cors.cc
 *
 */

#include "app_global_Cors.h"
#include <vector>
#include <iterator>
#include <sstream>
#include <string>
#include <drogon/drogon.h>

using namespace drogon;
using namespace app::global;

void Cors::initAndStart(const Json::Value &config)
{   
  
}


void Cors::setHeaders(
    const std::vector<CORSOption> &options,
    const HttpResponsePtr &response
) const {

    const auto cors_options = CORSOptions(options);
    cors_options.buildHeaders(response);

}


void Cors::shutdown() 
{
    /// Shutdown the plugin
}
