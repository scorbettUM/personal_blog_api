#include "api_v1_Posts.h"
#include <drogon/drogon.h>
#include <models/Posts.h>
using namespace drogon;
using namespace drogon_model;

using namespace api::v1;

// Add definition of your processing function here
Task<HttpResponsePtr> Posts::get(HttpRequestPtr req, std::string post_title) {
    
    (void)req;

    Json::Value data;
    auto db = drogon::app().getDbClient();
    drogon::orm::Mapper<drogon_model::sqlite3::Posts> posts_mapper(db); 

    try {
        data["post"] = posts_mapper.findFutureOne(
            drogon::orm::Criteria("name", drogon::orm::CompareOperator::EQ, post_title)
        ).get().toJson();

        auto resp = HttpResponse::newHttpJsonResponse(data);
        resp->setStatusCode(drogon::k200OK);

        LOG_INFO(logger, "GET - /api/v1/Posts/{} 200 OK", post_title);
        LOG_INFO(file_logger, "GET - /api/v1/Posts/{} 200 OK", post_title);
    
        co_return resp;

    } catch(...){

        data["post"] = "No posts found!";
        auto resp = HttpResponse::newHttpJsonResponse(data);
        resp->setStatusCode(drogon::k404NotFound);

        LOG_INFO(logger, "GET - /api/v1/Posts/{} 404 NOT FOUND", post_title);
        LOG_INFO(file_logger, "GET - /api/v1/Posts/{} 404 NOT FOUND", post_title);
    
        co_return resp;

    }

    
}   

Task<HttpResponsePtr> Posts::list(HttpRequestPtr req) {

    Json::Value data;
    auto db = drogon::app().getDbClient();
    drogon::orm::Mapper<drogon_model::sqlite3::Posts> posts_mapper(db);

    auto sort_field = req->getOptionalParameter<std::string>("sort_by").value_or("created");
    
    auto post_fields = drogon_model::sqlite3::Posts().toJson().getMemberNames();

    if (!std::count(post_fields.begin(), post_fields.end(), sort_field)){
        sort_field = "created";
    }

    auto posts_sort_order = req->getOptionalParameter<std::string>("sort_order").value_or("DESC");
    auto page = req->getOptionalParameter<int>("page").value_or(1);
    auto limit = req->getOptionalParameter<int>("sort_order").value_or(5);

    if (page < 1){
        page = 1;
    }

    if (limit < 1){
        limit = 1;
    }

    drogon::orm::SortOrder sort_order;

    if (posts_sort_order == "ASC"){
        sort_order = drogon::orm::SortOrder::ASC;

    } else {
        sort_order = drogon::orm::SortOrder::DESC;
    }


    try {
        auto posts = posts_mapper.orderBy(
            sort_field,
            sort_order 
        ).paginate(
            page,
            limit
        ).findFutureAll()
            .get();
            
        Json::Value posts_json;

        for (const auto &post : posts){
            posts_json.append(post.toJson());
        }

        data["posts"] = posts_json;

        auto resp=HttpResponse::newHttpJsonResponse(data);
        resp->setStatusCode(k200OK);

        LOG_INFO(
            logger, 
            "GET - /api/v1/Posts/lists?sort_by={}&sort_order={}&page={}&limit={} 200 OK",
            sort_field,
            posts_sort_order,
            page,
            limit
        );

        LOG_INFO(
            file_logger, 
            "GET - /api/v1/Posts/lists?sort_by={}&sort_order={}&page={}&limit={} 200 OK",
            sort_field,
            posts_sort_order,
            page,
            limit
        );

        co_return resp;
    } catch(...) {

        data["posts"] = {};
        auto resp=HttpResponse::newHttpJsonResponse(data);
        resp->setStatusCode(k404NotFound);

        LOG_INFO(
            logger, 
            "GET - /api/v1/Posts/lists?sort_by={}&sort_order={}&page={}&limit={} 404 NOT FOUND",
            sort_field,
            posts_sort_order,
            page,
            limit
        );

        LOG_INFO(
            file_logger, 
            "GET - /api/v1/Posts/lists?sort_by={}&sort_order={}&page={}&limit={} 404 NOT FOUND",
            sort_field,
            posts_sort_order,
            page,
            limit
        );

        co_return resp;
    }
}   