#pragma once
#include "CorsOption.h"
#include <drogon/drogon.h>


using namespace drogon;


class CORSOptions {
    public:

        CORSOptions(std::vector<CORSOption> cors_options){
            options = cors_options;

        }

        void buildHeaders(const HttpResponsePtr &response) const;

    private:
        std::vector<CORSOption> options;
};


