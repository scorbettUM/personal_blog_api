/**
 *
 *  auth_jwt_JWTAuthorizer.h
 *
 */

#pragma once

#include <drogon/plugins/Plugin.h>
#include <auth/jwt-cpp/jwt.h>

namespace auth
{
namespace jwt
{

class JWTAuthorizer : public drogon::Plugin<JWTAuthorizer>
{
  public:
    JWTAuthorizer() {}
    /// This method must be called by drogon to initialize and start the plugin.
    /// It must be implemented by the user.
    void initAndStart(const Json::Value &config) override;

    /// This method must be called by drogon to shutdown the plugin.
    /// It must be implemented by the user.
    void shutdown() override;
};

}
}
