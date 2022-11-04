#include "api_v1_Posts.h"

using namespace api::v1;

// Add definition of your processing function here
void Posts::get(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback, std::string post_title) const {

    Json::Value body;
    body[post_title] = "Hello!";

    auto resp=HttpResponse::newHttpJsonResponse(body);
    resp->setStatusCode(k200OK);

    callback(resp);
}   

void Posts::list(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback) const {

    Json::Value body;
    body["data"] = {"Hello!"};

    auto resp=HttpResponse::newHttpJsonResponse(body);
    resp->setStatusCode(k200OK);

    callback(resp);
}   