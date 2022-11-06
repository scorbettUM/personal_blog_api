#include "../../../../utilities/string/string_utils.h"
#include <vector>
#include <iostream>
#include <thread>
#include <fstream>
#include <cstring>
#include <chrono>
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
			Git(const RepoConfig &repo_config): config(repo_config)
			{

			}
			int operation_status;
			bool finished;

			void checkIfValidRepository();
			void setRemoteUrl(const std::string &remote_url);
			void setRepoPath(const std::string &repo_path);
			void init(const std::string &repo_path);
			void clone(const std::string &repo_url, const std::string &repo_path);
			void pull();
			void checkout(std::vector<std::string> options);
			void branch(std::vector<std::string> options);
			void fetch();
			void wait();
			void watch();
			void pause();
			int command_status;
			RepoConfig config;
			
		
		private:
			RunnerStatus runner_status = RUNNER_IDLE;
			std::thread watch_thread;
			std::vector<std::thread> threads;
			std::vector<GitJob> jobs;
			std::ostringstream current_command;
			RepoPathState path_state = REPO_NOT_SET;
			RemoteUrlState remote_url_state = REMOTE_URL_NOT_SET;
			void run_command();
	};

}