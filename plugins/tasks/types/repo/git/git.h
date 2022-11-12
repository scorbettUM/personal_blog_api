#pragma once
#include <vector>
#include <iostream>
#include <thread>
#include <fstream>
#include <cstring>
#include <chrono>
#include <filesystem>

#include "repo_config.h"
#include "git_command.h"




namespace task {

	namespace types {

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
				Git()
				{

				}
				Git(const RepoConfig &repo_config): config(repo_config)
				{

				}
				int operation_status;
				bool finished;

				uint64_t checkIfValidRepository(){

					auto command = GitCommand(
						CHECK_IF_REPO,
						command_id
					);

					command_id += 1;

					return command.run(
						std::string("git -C "),
						config.repo_path,
						std::string(" rev-parse --is-inside-work-tree"),
						std::string(" 2>/dev/null")
					);
				};

				uint64_t init() {

					auto command = GitCommand(
						CHECK_IF_REPO,
						command_id
					);

					command_id += 1;

					return command.run(
						std::string("git init "),
						config.repo_path,
						std::string(" 2>/dev/null")
					);
				};

				uint64_t clone() {

					auto command = GitCommand(
						CHECK_IF_REPO,
						command_id
					);

					command_id += 1;

					return command.run(
						std::string("git clone "),
						config.repo_url,
						std::string(" "),
						config.repo_path,
						std::string(" 2>/dev/null")
					);
					
				};

				uint64_t pull(){

					setRepoPath(config.repo_path);

					auto command = GitCommand(
						CHECK_IF_REPO,
						command_id
					);

					command_id += 1;

					uint64_t command_result =  command.run(
						std::string("git pull "),
						config.repo_remote,
						std::string(" "),
						config.repo_branch,
						std::string(" 2>/dev/null")
					);

					setRepoPath(home_path);

					return command_result;

				};
				
				uint64_t checkout(std::vector<std::string> options={}){

					std::string command_options;
					if (options.size() > 0){
						command_options = utilities::string::join(options, " ");

					} else {
						command_options = "";

					}

					setRepoPath(config.repo_path);

					auto command = GitCommand(
						CHECK_IF_REPO,
						command_id
					);

					command_id += 1;

					uint64_t command_result = command.run(
						std::string("git checkout "),
						command_options,
						config.repo_branch,
						std::string(" 2>/dev/null")
					);

					setRepoPath(home_path);

					return command_result;

				};
				uint64_t branch(std::vector<std::string> options){

					std::string command_options;
					if (options.size() > 0){
						command_options = utilities::string::join(options, " ");

					} else {
						command_options = "";

					}

					setRepoPath(config.repo_path);

					auto command = GitCommand(
						CHECK_IF_REPO,
						command_id
					);

					command_id += 1;

					uint64_t command_result =  command.run(
						std::string("git branch "),
						command_options,
						config.repo_branch,
						std::string(" 2>/dev/null")
					);


					setRepoPath(home_path);

					return command_result;
				};
				uint64_t fetch(){

					auto command = GitCommand(
						CHECK_IF_REPO,
						command_id
					);

					command_id += 1;

					uint64_t command_result = command.run(
						std::string("git fetch"),
						std::string(" 2>/dev/null")
					);

					setRepoPath(home_path);

					return command_result;
				};

				int command_status;
				RepoConfig config;
				
			
			private:
				uint64_t command_id = 1;
				void setRemoteUrl(const std::string &remote_url){

					config.repo_url = remote_url;
					remote_url_state = REMOTE_URL_SET;

				};
				void setRepoPath(const std::string &repo_path){
					auto path = std::filesystem::current_path(); //getting path
					std::filesystem::current_path(config.repo_path);
					config.repo_path = repo_path;

					path_state = REPO_SET;
				};
				std::string home_path = std::filesystem::current_path();
				RunnerStatus runner_status = RUNNER_IDLE;
				RepoPathState path_state = REPO_NOT_SET;
				RemoteUrlState remote_url_state = REMOTE_URL_NOT_SET;
				void run_command();
		};
	}

}