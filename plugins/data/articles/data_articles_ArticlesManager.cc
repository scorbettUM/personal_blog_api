/**
 *
 *  data_articles_ArticlesManager.cc
 *
 */
#include "data_articles_ArticlesManager.h"
#include <quill/Quill.h>

using namespace drogon;
using namespace data::articles;
using namespace manager;

void ArticlesManager::initAndStart(const Json::Value &config)
{

    auto logger_factory = utilities::logging::LoggerFactory();
    auto logger = logger_factory.createConsoleLogger("console");
    auto file_logger = logger_factory.createFileLogger("articles_job", "blog.articles.job.log");

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

            auto current_working_directory = std::filesystem::current_path().string();
            std::stringstream repo_filepath;
            repo_filepath << current_working_directory << "/posts";

            utilities::filesystem::create_file_at_path(repo_filepath.str());

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

    git_config = RepoConfig(
        repo_remote,
        repo_path,
        repo_branch,
        repo_url
    );

    runner = Git(git_config);

    // Initialize and start the plugin
    
    runner.clone();
    runner.wait();

    runner.fetch();
    runner.wait();

    run_job = true;

    runner_job = std::thread([&](){

        int article_pull_interval = 60;
        auto pull_interval = getenv("ARTICLES_PULL_INTERVAL");

        if (pull_interval != NULL){
            article_pull_interval = std::stoi((char*)pull_interval);
        } else if (!config["pull_interval"].isNull()){
            article_pull_interval = config["pull_interval"].asInt();
        }

        auto logger_factory = utilities::logging::LoggerFactory();
        auto job_logger = logger_factory.createConsoleLogger("console");
        auto job_file_logger = logger_factory.createFileLogger("articles_job", "blog.articles.job.log");

        LOG_INFO(job_logger, "Started Git -> Articles update job.");
        LOG_DEBUG(job_logger, "Git -> Articles update job: Running on process: {}", getpid());


        LOG_INFO(job_logger, "Started Git -> Articles update job.");
        LOG_INFO(job_file_logger, "Git -> Articles update job: Running on process: {}", getpid());

        std::unique_lock<std::mutex> lock(runner_mutex);
        
        while(run_job){

            try {
                std::vector<int> completed;

                LOG_DEBUG(job_logger, "Git -> Articles update job: Polling for completed pulls.");
                LOG_INFO(job_file_logger, "Git -> Articles update job: Polling for completed pulls.");

                for (GitJob &job : runner.jobs){
                    if (job.status == JOB_COMPLETE){

                        auto thread = std::move(runner.threads[job.job_idx]);

                        if(thread.joinable()){
                            thread.join();
                        }

                        completed.push_back(job.job_idx);
                    }
                }

                
                LOG_DEBUG(job_logger, "Git -> Articles update job: Completed {} pulls.", completed.size());
                LOG_INFO(job_file_logger, "Git -> Articles update job: Completed {} pulls.", completed.size());

                for (const auto &completed_idx : completed){
                    runner.jobs.erase(runner.jobs.begin() + completed_idx);
                    runner.threads.erase(runner.threads.begin() + completed_idx);
                }
            
                runner.pull();

                LOG_DEBUG(job_logger, "Git -> Articles update job: Pull complete.");
                LOG_INFO(job_file_logger, "Git -> Articles update job: Pull complete.");

            } catch(...){
                LOG_CRITICAL(job_logger, "Git -> Articles update job: Encountered critical error. Restarting.");
                LOG_CRITICAL(job_file_logger, "Git -> Articles update job: Encountered critical error. Restarting.");
            }

            {   
                runner_conditional.wait_for(
                    lock, 
                    std::chrono::duration(
                        std::chrono::seconds(article_pull_interval)
                    ),
                    [&](){
                        return !run_job;
                    }    
                );
            }
            
        }

        if(!run_job){
            runner.stop();
        }

    });


}


void ArticlesManager::shutdown() 
{   
    auto logger_factory = utilities::logging::LoggerFactory();
    auto logger = logger_factory.createConsoleLogger("console");
    auto file_logger = logger_factory.createFileLogger("articles_job", "blog.articles.job.log");

    run_job = false;
    runner_conditional.notify_all();


    LOG_INFO(logger, "Git -> Articles update job has been notified of shutdown. Please wait...");
    LOG_INFO(file_logger, "Git -> Articles update job has been notified of shutdown. Please wait...");

    runner_job.join();
    runner.threads.clear();
    runner.jobs.clear();

    LOG_INFO(logger, "Git -> Articles update job has stopped.");
    LOG_INFO(file_logger, "Git -> Articles update job has stopped.");
    
}
