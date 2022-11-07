/**
 *
 *  data_articles_ArticlesManager.cc
 *
 */
#include "data_articles_ArticlesManager.h"

using namespace drogon;
using namespace data::articles;
using namespace manager;

void ArticlesManager::initAndStart(const Json::Value &config)
{

    int article_pull_interval;
    auto pull_interval = config["process_interval"];
    if (!pull_interval){

        auto pull_interval_env = getenv("ARTICLE_PULL_INTERVAL");
        if (pull_interval_env == NULL){
            article_pull_interval = 60;
        } else {
            article_pull_interval = std::stoi((char*)pull_interval_env);
        }

    } else {
        article_pull_interval = pull_interval.asInt();
    }

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
        while(run_job){


            std::vector<int> completed;
            for (GitJob &job : runner.jobs){
                if (job.status == JOB_COMPLETE){

                        auto thread = std::move(runner.threads[job.job_idx]);

                    if(thread.joinable()){
                            thread.join();
                        }

                        completed.push_back(job.job_idx);
                }
            }

            for (const auto &completed_idx : completed){
                runner.jobs.erase(runner.jobs.begin() + completed_idx);
                runner.threads.erase(runner.threads.begin() + completed_idx);
            }
        
            runner.pull();

            {
                std::unique_lock<std::mutex> lock(runner_mutex);
                runner_conditional.wait_for(lock, std::chrono::seconds(article_pull_interval));
            }
        }

        if(!run_job){
            runner.stop();
        }

        return 0;

    });


}


void ArticlesManager::shutdown() 
{
    run_job = false;
    runner_conditional.notify_all();

    runner_job.join();
    runner.threads.clear();
    runner.jobs.clear();
    /// Shutdown the plugin
    
}
