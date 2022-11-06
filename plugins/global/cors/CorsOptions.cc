#include "CorsOptions.h"


void CORSOptions::buildHeaders(const HttpResponsePtr &response) const {
    
    for (const auto &option : options){
        response->addHeader(option.name, option.toValues());
    }

};