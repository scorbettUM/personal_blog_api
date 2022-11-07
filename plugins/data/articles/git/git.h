#include "../../../../utilities/string/string_utils.h"
#include <vector>
#include <iostream>
#include <thread>
#include <fstream>
#include <cstring>
#include <chrono>
#include <filesystem>

#include "repo_config.h"
#include "git_job.h"




namespace manager {

	enum RunnerStatus {
		RUNNER_IDLE,
		RUNNER_EXECUTING,
		RUNNER_STOPPED
	};

	enum RepoPathState {
		REPO_SET,
		REPO_NOT_SET
	};


	enum RemoteUrlState {
		REMOTE_URL_SET,
		REMOTE_URL_NOT_SET
	};
	
	class Git {

		public:
			Git(){

			}
			Git(const RepoConfig &repo_config): config(repo_config)
			{

			}
			int operation_status;
			bool finished;

			void checkIfValidRepository();
			void init();
			void clone();
			void pull();
			void runPullJob();
			void checkout(std::vector<std::string> options);
			void branch(std::vector<std::string> options);
			void fetch();
			void wait();
			void stop();
			int command_status;
			RepoConfig config;
			std::vector<std::thread> threads;
			std::vector<GitJob> jobs;
			
		
		private:
			void setRemoteUrl(const std::string &remote_url);
			void setRepoPath(const std::string &repo_path);
			std::string home_path = std::filesystem::current_path();
			RunnerStatus runner_status = RUNNER_IDLE;
			RepoPathState path_state = REPO_NOT_SET;
			RemoteUrlState remote_url_state = REMOTE_URL_NOT_SET;
			void run_command();
	};

}