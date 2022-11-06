/**
 *
 *  data_markdown_ConvertMarkdownToHTML.cc
 *
 */

#include "data_markdown_ConvertMarkdownToHTML.h"
#include <drogon/drogon.h>
using namespace drogon;
using namespace data::markdown;

void ConvertMarkdownToHTML::initAndStart(const Json::Value &config)
{
    /// Initialize and start the plugin
}

void ConvertMarkdownToHTML::convertMarkdownToHTML() const {

    auto markdown_path = getenv("MARKDOWN_PATH");
    if (markdown_path == NULL) {
        markdown_path = (char *)"../markdown";
    }

    std::stringstream markdown;
    std::shared_ptr<maddy::ParserConfig> config = std::make_shared<maddy::ParserConfig>();
    config->isEmphasizedParserEnabled = true; // default
    config->isHTMLWrappedInParagraph = true; // default

    std::shared_ptr<maddy::Parser> parser = std::make_shared<maddy::Parser>(config);
    std::string htmlOutput = parser->Parse(markdown);
}

void ConvertMarkdownToHTML::shutdown() 
{
    /// Shutdown the plugin
}
