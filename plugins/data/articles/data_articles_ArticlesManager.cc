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

    // auto article_pull_interval = std::stoi(pull_interval);

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
