#include "api_v1_Categories.h"
#include <drogon/drogon.h>
#include <models/Tags.h>
#include <models/Posts.h>
#include <models/Categories.h>

using namespace api::v1;
using namespace drogon;
using namespace drogon_model;

// Add definition of your processing function here
Task<HttpResponsePtr> Categories::list(HttpRequestPtr req) {

    Json::Value data;
    auto db = drogon::app().getDbClient();
    drogon::orm::Mapper<drogon_model::sqlite3::Categories> categories_mapper(db); 

    auto sort_field = req->getOptionalParameter<std::string>("sort_by").value_or("created");
    
    auto categories_fields = drogon_model::sqlite3::Categories().toJson().getMemberNames();

    if (!std::count(categories_fields.begin(), categories_fields.end(), sort_field)){
        sort_field = "created";
    }

    auto categories_sort_order = req->getOptionalParameter<std::string>("sort_order").value_or("DESC");
    auto page = req->getOptionalParameter<int>("page").value_or(1);
    auto limit = req->getOptionalParameter<int>("limit").value_or(5);

    if (page < 1){
        page = 1;
    }

    if (limit < 1){
        limit = 1;
    }
    
    drogon::orm::SortOrder sort_order;

    if (categories_sort_order == "ASC"){
        sort_order = drogon::orm::SortOrder::ASC;

    } else {
        sort_order = drogon::orm::SortOrder::DESC;
    }

    try {
        auto categories = categories_mapper.orderBy(
            sort_field,
            sort_order 
        ).paginate(
            page,
            limit
        ).findFutureAll()
            .get();
            
        Json::Value categories_json;

        for (const auto &category : categories){
            auto category_json = category.toJson();
            categories_json.append(category_json);
        }
        
        data["categories"] = categories_json;
        if (categories_json.empty()){

            data["categories"] = Json::Value(Json::arrayValue);

        }

        auto resp=HttpResponse::newHttpJsonResponse(data);
        resp->setStatusCode(k200OK);

        LOG_INFO(
            logger, 
            "GET - /api/v1/Categories/list?sort_by={}&sort_order={}&page={}&limit={} 200 OK",
            sort_field,
            categories_sort_order,
            page,
            limit
        );

        LOG_INFO(
            file_logger, 
            "GET - /api/v1/Categories/list?sort_by={}&sort_order={}&page={}&limit={} 200 OK",
            sort_field,
            categories_sort_order,
            page,
            limit
        );

        co_return resp;
        
    } catch(std::exception& e) {

        data["categories"] = Json::Value(Json::arrayValue);
        data["error"] = e.what();
        auto resp=HttpResponse::newHttpJsonResponse(data);
        resp->setStatusCode(k404NotFound);

        LOG_INFO(
            logger, 
            "GET - /api/v1/Categories/list?sort_by={}&sort_order={}&page={}&limit={} 404 NOT FOUND",
            sort_field,
            categories_sort_order,
            page,
            limit
        );

        LOG_INFO(
            file_logger, 
            "GET - /api/v1/Categories/list?sort_by={}&sort_order={}&page={}&limit={} 404 NOT FOUND",
            sort_field,
            categories_sort_order,
            page,
            limit
        );

        co_return resp;
    }

}