/**
 *
 *  data_markdown_ConvertMarkdownToHTML.cc
 *
 */

#include "data_markdown_ConvertMarkdownToHTML.h"
#include <drogon/drogon.h>
#include <models/Posts.h>
using namespace drogon;
using namespace drogon_model;
using namespace data::markdown;

void ConvertMarkdownToHTML::initAndStart(const Json::Value &config)
{
    int article_process_interval;
    auto process_interval = config["process_interval"];
    if (!process_interval){

        auto process_interval_env = getenv("ARTICLE_PROCESS_INTERVAL");
        if (process_interval_env == NULL){
            article_process_interval = 60;
        } else {
            article_process_interval = std::stoi((char*)process_interval_env);
        }

    } else {
        article_process_interval = process_interval.asInt();
    }

    /// Initialize and start the plugin
    auto config_url = config["repo_url"];
    if (!config_url){
        throw "Err. - You must specify a repo url!";
    }

    auto config_path = config["repo_path"];
    if (!config_path){
        throw "Err. - You must specify a repo path";
    }


    auto config_branch = config["repo_branch"];
    if (!config_branch){
        config_branch = std::string("main");
    }

    auto config_remote = config["repo_remote"];
    if (!config_remote ){
        config_remote = std::string("origin");
    }

    auto repo_remote = std::string(config_remote.asCString());
    auto repo_url = std::string(config_url.asCString());
    auto repo_path = std::string(config_path.asCString());
    auto repo_branch = std::string(config_branch.asCString());

    repo_config = manager::RepoConfig(
        repo_remote,
        repo_path,
        repo_branch,
        repo_url
    );

    run_job = true;

    markdown_job = std::thread([&](){

        int start_offset = rand() % 4 + 5;

        std::this_thread::sleep_for(
            std::chrono::seconds(start_offset)
        );

        auto db = drogon::app().getDbClient();
        drogon::orm::Mapper<drogon_model::sqlite3::Posts> posts_mapper(db);

        while(run_job){

            auto articles_output_path = getenv("ARTICLES_OUTPUT_PATH");
            if (articles_output_path == NULL){
                articles_output_path = (char *)"../articles";
            }
      
            std::string ext(".md");
            
            std::vector<std::pair<std::string, std::stringstream>> markdown_files;

            for (auto &file : std::filesystem::recursive_directory_iterator(repo_config.repo_path))
            {

                std::string article_name = file.path().stem().string();

                auto record_count = posts_mapper.count(
                    drogon::orm::Criteria("name", drogon::orm::CompareOperator::EQ, article_name)
                );
       
                if (file.path().extension() == ext && record_count == 0){

                    std::stringstream buffer;
                    std::ifstream ifs(file.path().string());       // note no mode needed

                    if ( !ifs.is_open() ) {                 
                        std::cout <<" Failed to open article at: "<< file.path().string()<< std::endl;
                    }
                    else {
                
                        buffer << ifs.rdbuf();

                        markdown_files.push_back(
                            std::pair(
                                file.path().stem().string(),
                                std::move(buffer)
                            )
                        );

                    }
                }
            }

            std::vector<std::pair<std::string, std::string>> articles_html;

            std::shared_ptr<maddy::ParserConfig> config = std::make_shared<maddy::ParserConfig>();
            config->isEmphasizedParserEnabled = true; // default
            config->isHTMLWrappedInParagraph = true; // default

            std::shared_ptr<maddy::Parser> parser = std::make_shared<maddy::Parser>(config);
            
            for(std::pair<std::string, std::stringstream> &markdown_file : markdown_files){

                Json::Value post_data;
                post_data["post_id"] = uuid::generate_uuid_v4();
                post_data["name"] = markdown_file.first;
                post_data["body"] = parser->Parse(markdown_file.second);

                auto post =drogon_model::sqlite3::Posts(post_data);


                posts_mapper.insertFuture(post);

            }

            {
                std::unique_lock<std::mutex> lock(runner_mutex);
                runner_conditional.wait_for(lock, std::chrono::seconds(article_process_interval));
            }
        }

        return 0;

    });

}


void ConvertMarkdownToHTML::shutdown() 
{
    /// Shutdown the plugin
    run_job = false;
    runner_conditional.notify_all();

    markdown_job.join();
}
