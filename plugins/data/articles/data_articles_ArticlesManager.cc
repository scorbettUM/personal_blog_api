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

    auto repo_config = RepoConfig(
        repo_remote,
        repo_path,
        repo_branch,
        repo_url
    );


    std::cout<<repo_config.repo_url<<std::endl;
    // Initialize and start the plugin
    auto git_manager = Git(repo_config);

    git_manager.watch();
    
    git_manager.checkIfValidRepository();

    std::cout<<"HERE!"<<git_manager.command_status<<std::endl;

    if (git_manager.command_status > 0){
        git_manager.clone(repo_url, repo_path);
    }

    git_manager.pause();
    
    // git_manager.pause();
}


void ArticlesManager::updateArticles() const {

    auto config = drogon::app().getCustomConfig();

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
    
    auto repo_config = RepoConfig(
        repo_remote,
        repo_path,
        repo_branch,
        repo_url
    );
    /// Initialize and start the plugin
    // auto git_manager = Git(repo_config);

    // git_manager.setRepoPath(repo_path);
    // git_manager.setRemoteUrl(repo_url);

    // git_manager.pull();

}

void ArticlesManager::shutdown() 
{
    /// Shutdown the plugin
}
