#include "git.h"

using namespace manager;


void Git::checkIfValidRepository(){

	runner_status = RUNNER_EXECUTING;


	auto job_thread = std::thread([&](){
		
		auto job = GitJob(
			CHECK_IF_REPO,
			jobs.size()
		);

		auto job_id = job.run(
			std::string("git -C "),
			config.repo_path,
			std::string(" rev-parse --is-inside-work-tree")
		);

		jobs.push_back(
			std::move(job)
		);
	});

	threads.push_back(
		std::move(job_thread)
	);
	
}


void Git::setRemoteUrl(const std::string &remote_url){

	config.repo_url = remote_url;
	remote_url_state = REMOTE_URL_SET;

}


void Git::setRepoPath(const std::string &repo_path){
	auto path = std::filesystem::current_path(); //getting path
    std::filesystem::current_path(config.repo_path);
	config.repo_path = repo_path;

	path_state = REPO_SET;
}


void Git::init() {


	runner_status = RUNNER_EXECUTING;


	auto job_thread = std::thread([&]() mutable {
		
		auto job = GitJob(
			CHECK_IF_REPO,
			jobs.size()
		);

		auto job_id = job.run(
			std::string("git init "),
			config.repo_path
		);

		jobs.push_back(
			std::move(job)
		);
	});

	threads.push_back(
		std::move(job_thread)
	);


}


void Git::clone() {

	runner_status = RUNNER_EXECUTING;


	auto job_thread = std::thread([&]() mutable {
		
		auto job = GitJob(
			CHECK_IF_REPO,
			jobs.size()
		);

		auto job_id = job.run(
			std::string("git clone "),
			config.repo_url,
			std::string(" "),
			config.repo_path
		);

		jobs.push_back(
			std::move(job)
		);
	});

	threads.push_back(
		std::move(job_thread)
	);

	
}


void Git::pull(){


	runner_status = RUNNER_EXECUTING;

	setRepoPath(config.repo_path);


	auto job_thread = std::thread([&]() mutable {
		
		auto job = GitJob(
			CHECK_IF_REPO,
			jobs.size()
		);

		auto job_id = job.run(
			std::string("git pull "),
			config.repo_remote,
			std::string(" "),
			config.repo_branch
		);

		jobs.push_back(
			std::move(job)
		);

		setRepoPath(home_path);
	});

	threads.push_back(
		std::move(job_thread)
	);

}


void Git::checkout(std::vector<std::string> options){

	std::string command_options;
	if (options.size() > 0){
		command_options = join(options, " ");

	} else {
		command_options = "";

	}

	runner_status = RUNNER_EXECUTING;

	setRepoPath(config.repo_path);

	auto job_thread = std::thread([&]() mutable {
		
		auto job = GitJob(
			CHECK_IF_REPO,
			jobs.size()
		);

		auto job_id = job.run(
			std::string("git checkout "),
			command_options,
			config.repo_branch
		);

		jobs.push_back(
			std::move(job)
		);

		setRepoPath(home_path);
	});

	threads.push_back(
		std::move(job_thread)
	);

}


void Git::branch(std::vector<std::string> options){

	std::string command_options;
	if (options.size() > 0){
		command_options = join(options, " ");

	} else {
		command_options = "";

	}

	runner_status = RUNNER_EXECUTING;

	setRepoPath(config.repo_path);

	auto job_thread = std::thread([&]() mutable {
		
		auto job = GitJob(
			CHECK_IF_REPO,
			jobs.size()
		);

		auto job_id = job.run(
			std::string("git branch "),
			command_options,
			config.repo_branch
		);

		jobs.push_back(
			std::move(job)
		);

		setRepoPath(home_path);

	});

	threads.push_back(
		std::move(job_thread)
	);
}


void Git::fetch(){

	runner_status = RUNNER_EXECUTING;

	auto job_thread = std::thread([&]() mutable {
		
		auto job = GitJob(
			CHECK_IF_REPO,
			jobs.size()
		);

		auto job_id = job.run(
			std::string("git fetch")
		);

		jobs.push_back(
			std::move(job)
		);

		setRepoPath(home_path);
	});

	threads.push_back(
		std::move(job_thread)
	);

}


void Git::wait(){
	std::thread &thread = threads.back();
	
	if (thread.joinable()){
		thread.join();
	}

	if (jobs.size() > 0){
		GitJob &job = jobs.back();
		jobs.erase(jobs.begin() + job.job_idx);
	}
}

