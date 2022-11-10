#pragma once
#include <string>


namespace manager {
    class RepoConfig {
        public:    
            RepoConfig(){
                
            }     
            RepoConfig(
                std::string remote,
                std::string path,
                std::string branch,
                std::string url
            ){

                repo_remote = remote;
                repo_path = path;
                repo_branch = branch;
                repo_url = url;

            }

            std::string repo_remote;
            std::string repo_path;
            std::string repo_branch;
            std::string repo_url;
            void stuff();
    };

    class Bleh {

        Bleh(){}

    };
}