#include "api_v1_Posts.h"
#include <drogon/drogon.h>
#include <models/Posts.h>
using namespace drogon;
using namespace drogon_model;

using namespace api::v1;

// Add definition of your processing function here
Task<HttpResponsePtr> Posts::get(HttpRequestPtr req, std::string post_title) {

    Json::Value data;
    auto db = drogon::app().getDbClient();
    drogon::orm::Mapper<drogon_model::sqlite3::Posts> posts_mapper(db); 

    try {
        data["post"] = posts_mapper.findFutureOne(
            drogon::orm::Criteria("name", drogon::orm::CompareOperator::EQ, post_title)
        ).get().toJson();

        auto resp = HttpResponse::newHttpJsonResponse(data);
        resp->setStatusCode(drogon::k200OK);
    
        co_return resp;

    } catch(...){

        data["post"] = "No posts found!";
        auto resp = HttpResponse::newHttpJsonResponse(data);
        resp->setStatusCode(drogon::k404NotFound);
    
        co_return resp;

    }

    
}   

Task<HttpResponsePtr> Posts::list(HttpRequestPtr req) {

    Json::Value data;
    auto db = drogon::app().getDbClient();
    drogon::orm::Mapper<drogon_model::sqlite3::Posts> posts_mapper(db);

    auto sort_field = req->getOptionalParameter<std::string>("sort_by").value_or("created");
    auto sort_order = req->getOptionalParameter<std::string>("sort_order").value_or("DESC");
    auto limit = req->getOptionalParameter<int>("sort_order").value_or(5);

    
    std::cout<<"Sorting by: "<<sort_field<<" order: "<<sort_order<<std::endl;


    try {
        data["posts"] = posts_mapper.findFutureAll().get().data()->toJson();

        auto resp=HttpResponse::newHttpJsonResponse(data);
        resp->setStatusCode(k200OK);

        co_return resp;
    } catch(...) {

        data["posts"] = {};
        auto resp=HttpResponse::newHttpJsonResponse(data);
        resp->setStatusCode(k404NotFound);

        co_return resp;
    }
}   