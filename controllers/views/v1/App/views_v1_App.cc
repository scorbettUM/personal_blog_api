#include "views_v1_App.h"
#include <drogon/drogon.h>
#include <models/Posts.h>
using namespace drogon;
using namespace drogon_model;

using namespace views::v1;

// Add definition of your processing function here
Task<HttpResponsePtr> App::get_home(HttpRequestPtr req) {

    auto db = drogon::app().getDbClient();
    drogon::orm::Mapper<drogon_model::sqlite3::Posts> posts_mapper(db); 

    auto posts = posts_mapper.findFutureAll().get();

    std::cout<<posts.data()->toJson()["body"]<<std::endl;

    drogon::HttpViewData data;
    data.insert("posts", posts.data()->toJson()["body"]);

    auto resp = HttpResponse::newHttpViewResponse("Home.csp", data);
    resp->setStatusCode(drogon::k200OK);
 

    co_return resp;
}


Task<HttpResponsePtr> App::get_post(HttpRequestPtr req, const std::string &post_name) {

    auto resp = HttpResponse::newHttpResponse();
    resp->setStatusCode(drogon::k200OK);

    co_return resp;
}