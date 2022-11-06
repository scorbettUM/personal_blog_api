/**
 *
 *  data_markdown_ConvertMarkdownToHTML.h
 *
 */

#pragma once

#include <drogon/plugins/Plugin.h>
#include "parser/parser.h"
#include <string>
#include <cstdlib>

namespace data
{
namespace markdown
{

class ConvertMarkdownToHTML : public drogon::Plugin<ConvertMarkdownToHTML>
{
  public:
    ConvertMarkdownToHTML() {}
    /// This method must be called by drogon to initialize and start the plugin.
    /// It must be implemented by the user.
    void initAndStart(const Json::Value &config) override;
    void convertMarkdownToHTML() const ;
    /// This method must be called by drogon to shutdown the plugin.
    /// It must be implemented by the user.
    void shutdown() override;
};

}
}
