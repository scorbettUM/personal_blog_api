#include "api_v1_Tags.h"
#include <drogon/drogon.h>
#include <models/Tags.h>
#include <models/Posts.h>
#include <models/Categories.h>

using namespace api::v1;
using namespace drogon;
using namespace drogon_model;

// Add definition of your processing function here
Task<HttpResponsePtr> Tags::list(HttpRequestPtr req) {

    Json::Value data;
    auto db = drogon::app().getDbClient();
    drogon::orm::Mapper<drogon_model::sqlite3::Tags> tags_mapper(db); 

    auto sort_field = req->getOptionalParameter<std::string>("sort_by").value_or("created");
    
    auto tag_fields = drogon_model::sqlite3::Tags().toJson().getMemberNames();

    if (!std::count(tag_fields.begin(), tag_fields.end(), sort_field)){
        sort_field = "created";
    }

    auto tags_sort_order = req->getOptionalParameter<std::string>("sort_order").value_or("DESC");
    auto page = req->getOptionalParameter<int>("page").value_or(1);
    auto limit = req->getOptionalParameter<int>("limit").value_or(5);

    if (page < 1){
        page = 1;
    }

    if (limit < 1){
        limit = 1;
    }
    
    drogon::orm::SortOrder sort_order;

    if (tags_sort_order == "ASC"){
        sort_order = drogon::orm::SortOrder::ASC;

    } else {
        sort_order = drogon::orm::SortOrder::DESC;
    }

    try {
        auto tags = tags_mapper.orderBy(
            sort_field,
            sort_order 
        ).paginate(
            page,
            limit
        ).findFutureAll()
            .get();
            
        Json::Value tags_json;

        for (const auto &tag : tags){
            auto tag_json = tag.toJson();
            tags_json.append(tag_json);
        }
        
        data["tags"] = tags_json;
        if (tags_json.empty()){

            data["tags"] = Json::Value(Json::arrayValue);

        }

        auto resp=HttpResponse::newHttpJsonResponse(data);
        resp->setStatusCode(k200OK);

        LOG_INFO(
            logger, 
            "GET - /api/v1/Tags/list?sort_by={}&sort_order={}&page={}&limit={} 200 OK",
            sort_field,
            tags_sort_order,
            page,
            limit
        );

        LOG_INFO(
            file_logger, 
            "GET - /api/v1/Tags/list?sort_by={}&sort_order={}&page={}&limit={} 200 OK",
            sort_field,
            tags_sort_order,
            page,
            limit
        );

        co_return resp;
        
    } catch(std::exception& e) {

        data["tags"] = Json::Value(Json::arrayValue);
        data["error"] = e.what();
        auto resp=HttpResponse::newHttpJsonResponse(data);
        resp->setStatusCode(k404NotFound);

        LOG_INFO(
            logger, 
            "GET - /api/v1/Tags/list?sort_by={}&sort_order={}&page={}&limit={} 404 NOT FOUND",
            sort_field,
            tags_sort_order,
            page,
            limit
        );

        LOG_INFO(
            file_logger, 
            "GET - /api/v1/Tags/list?sort_by={}&sort_order={}&page={}&limit={} 404 NOT FOUND",
            sort_field,
            tags_sort_order,
            page,
            limit
        );

        co_return resp;
    }

}