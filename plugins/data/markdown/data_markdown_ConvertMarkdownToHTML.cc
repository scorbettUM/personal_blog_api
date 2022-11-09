/**
 *
 *  data_markdown_ConvertMarkdownToHTML.cc
 *
 */

#include "data_markdown_ConvertMarkdownToHTML.h"

using namespace drogon;
using namespace drogon_model;
using namespace data::markdown;

void ConvertMarkdownToHTML::initAndStart(const Json::Value &config)
{

    auto logger_factory = utilities::logging::LoggerFactory();
    auto logger = logger_factory.createConsoleLogger("console");
    auto file_logger = logger_factory.createFileLogger("markdown_job", "blog.markdown.job.log");

    /// Initialize and start the plugin

    auto repo_url = config["repo_url"].asString();
    if (repo_url.size() == 0){

        auto config_url_env = getenv("REPO_URL");
        if (config_url_env == NULL){
            
            throw std::runtime_error("Err. - You must specify a repo url!");   
        } else {
            repo_url = std::string((char*)config_url_env);
        }

    }

    auto repo_path = config["repo_path"].asString();
    if (repo_path.size() == 0){
         auto config_path_env = getenv("REPO_PATH");
        if (config_path_env == NULL){
            throw std::runtime_error("Err. - You must specify a repo path!");   
        } else {
            repo_path = std::string((char*)config_path_env);
        }

    }

    auto repo_branch = config["repo_branch"].asString();
    if (repo_branch.size() == 0){

         auto config_branch_env = getenv("REPO_BRANCH");
        if (config_branch_env == NULL){
            repo_branch = "main";   
        } else {
            repo_branch = std::string((char*)config_branch_env);
        }

    }

    auto repo_remote = config["repo_remote"].asString();
    if (repo_remote.size() ==  0){

        auto config_remote_env = getenv("REPO_REMOTE");
        if (config_remote_env == NULL){
            repo_remote = "origin";
               
        } else {
            repo_remote = std::string((char*)config_remote_env);
        }

    }

    LOG_DEBUG(logger, "Markdown -> HTML converter job: Targeting repository url {}", repo_url);
    LOG_DEBUG(logger, "Markdown -> HTML converter job: Targeting repository remote {}", repo_remote);
    LOG_DEBUG(logger, "Markdown -> HTML converter job: Targeting repository branch {}", repo_branch);
    LOG_DEBUG(logger, "Markdown -> HTML converter job: Targeting repository path {}", repo_path);


    LOG_INFO(file_logger, "Markdown -> HTML converter job: Targeting repository url {}", repo_url);
    LOG_INFO(file_logger, "Markdown -> HTML converter job: Targeting repository remote {}", repo_remote);
    LOG_INFO(file_logger, "Markdown -> HTML converter job: Targeting repository branch {}", repo_branch);
    LOG_INFO(file_logger, "Markdown -> HTML converter job: Targeting repository path {}", repo_path);

    repo_config = manager::RepoConfig(
        repo_remote,
        repo_path,
        repo_branch,
        repo_url
    );

    run_job = true;

    markdown_job = std::thread([&](){

        int article_process_interval = 60;
        auto process_interval = getenv("ARTICLE_PROCESS_INTERVAL");

        if (process_interval != NULL){
            article_process_interval = std::stoi((char*)process_interval);

        } else if (!config["process_interval"].isNull()) {
            article_process_interval = config["process_interval"].asInt();
        }

        auto logger_factory = utilities::logging::LoggerFactory();
        auto job_logger = logger_factory.createConsoleLogger("console");
        auto job_file_logger = logger_factory.createFileLogger("markdown_job", "blog.markdown.job.log");

        LOG_INFO(job_logger, "Started Markdown -> HTML converter job.");
        LOG_DEBUG(job_logger, "Markdown -> HTML converter job: Running on process: {}", getpid());

        LOG_INFO(job_file_logger, "Started Markdown -> HTML converter job.");
        LOG_INFO(job_file_logger, "Markdown -> HTML converter job: Running on process: {}", getpid());

        int start_offset = rand() % article_process_interval + article_process_interval;

        LOG_DEBUG(job_logger, "Markdown -> HTML converter job: Starting in: {} seconds.", start_offset);
        LOG_DEBUG(job_file_logger, "Markdown -> HTML converter job: Starting in: {} seconds.", start_offset);

        std::unique_lock<std::mutex> lock(runner_mutex);
        runner_conditional.wait_for(lock, std::chrono::seconds(start_offset));


        auto db = drogon::app().getDbClient();
        drogon::orm::Mapper<drogon_model::sqlite3::Posts> posts_mapper(db);

        while(run_job){

            try {
                std::string ext(".md");
                
                std::vector<std::pair<std::string, std::stringstream>> markdown_files;

                for (auto &file : std::filesystem::recursive_directory_iterator(repo_config.repo_path))
                {

                    std::string article_name = file.path().stem().string();

                    auto record_count = posts_mapper.countFuture(
                        drogon::orm::Criteria("name", drogon::orm::CompareOperator::EQ, article_name)
                    ).get();
        
                    if (file.path().extension() == ext && record_count == 0){

                        std::stringstream buffer;
                        std::ifstream ifs(file.path().string());       // note no mode needed

                        if ( !ifs.is_open() ) { 
                            LOG_WARNING(job_logger, "Markdown -> HTML converter job: Failed to open article at: {}", file.path().string());  
                            LOG_WARNING(job_file_logger, "Markdown -> HTML converter job: Failed to open article at: {}", file.path().string());   

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

                LOG_DEBUG(job_logger, "Markdown -> HTML converter job: Discovered: {} new articles.", markdown_files.size());
                LOG_INFO(job_file_logger, "Markdown -> HTML converter job: Discovered: {} new articles.", markdown_files.size());

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


                    posts_mapper.insertFuture(post).get();

                }

                LOG_DEBUG(job_logger, "Markdown -> HTML converter job: Saved: {} new articles.", markdown_files.size());
                LOG_INFO(job_file_logger, "Markdown -> HTML converter job: Saved: {} new articles.", markdown_files.size());
                
                runner_conditional.wait_for(
                    lock, 
                    std::chrono::duration(
                        std::chrono::seconds(article_process_interval)
                    ),  
                    [&](){
                        return !run_job;
                    }
                );
            } catch(...){
                LOG_CRITICAL(job_logger, "Markdown -> HTML converter job: Encountered critical error. Restarting.");
                LOG_CRITICAL(job_file_logger, "Markdown -> HTML converter job: Encountered critical error. Restarting.");
            }
        }

    });

}


void ConvertMarkdownToHTML::shutdown() 
{
    /// Shutdown the plugin
    auto logger_factory = utilities::logging::LoggerFactory();
    auto logger = logger_factory.createConsoleLogger("console");
    auto file_logger = logger_factory.createFileLogger("markdown_job", "blog.markdown.job.log");
    
    run_job = false;
    runner_conditional.notify_all();

    LOG_INFO(logger, "Markdown -> HTML Converter job has been notified of shutdown. Please wait...");
    LOG_INFO(file_logger, "Markdown -> HTML Converter job has been notified of shutdown. Please wait...");

    if(markdown_job.joinable()){
        markdown_job.join();
    }

    LOG_INFO(logger, "Markdown -> HTML Converter job has stopped.");
    LOG_INFO(file_logger, "Markdown -> HTML Converter job has stopped.");
}
