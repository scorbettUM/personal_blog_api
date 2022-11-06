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

	// SetCurrentDirectory(config.repo_path);
	// config.repo_path = repo_path;

	// path_state = REPO_SET;
}


void Git::init(const std::string &repo_path) {

	// git_state = RUNNING;

	// if (path_state == REPO_NOT_SET){
	// 	config.repo_path = repo_path;
	// 	path_state = REPO_SET;
	// }

	// auto task = std::thread([&](){

   	//  	current_command << "git init " << config.repo_path;

	// 	run_command();

	// 	git_state = COMPLETE;
		
	// });

	// tasks.push_back(task);

}


void Git::clone(const std::string &repo_url, const std::string &repo_path) {

	runner_status = RUNNER_EXECUTING;


	auto job_thread = std::thread([&](){
		
		auto job = GitJob(
			CHECK_IF_REPO,
			jobs.size()
		);

		auto job_id = job.run(
			std::string("git clone "),
			config.repo_url,
			" ",
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

	// git_state = RUNNING;

	// auto task = std::thread([&](){

	// 	current_command << "git pull " << config.repo_remote << " " << config.repo_branch;

	// 	run_command();

	// 	git_state = COMPLETE;

	// });

	// tasks.push_back(task);

}


void Git::checkout(std::vector<std::string> options){
	// git_state = RUNNING;

	// auto task = std::thread([&](){

	// 	std::string command_options;
	// 	if (options.size() > 0){
	// 		command_options = join(options, " ");

	// 	} else {
	// 		command_options = "";

	// 	}

	// 	current_command << "git checkout " << command_options << config.repo_branch;

	// 	run_command();

	// 	git_state = COMPLETE;

	// });

	// tasks.push_back(task);

}


void Git::branch(std::vector<std::string> options){
	// git_state = RUNNING;

	// auto task = std::thread([&](){

	// 	std::string command_options;
	// 	if (options.size() > 0){
	// 		command_options = join(options, " ");

	// 	} else {
	// 		command_options = "";
			
	// 	}

	// 	current_command << "git branch " << config.repo_branch;

	// 	run_command();

	// 	git_state = COMPLETE;

	// });

	// tasks.push_back(task);

}


void Git::fetch(){
	// git_state = RUNNING;

	// auto task = std::thread([&](){

	// 	current_command = std::string("git fetch");

	// 	run_command();

	// 	git_state = COMPLETE;

	// });

	// tasks.push_back(task);

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

void Git::watch(){

	if (!watch_thread.joinable()){

		watch_thread = std::thread([&](){
			while(runner_status == RUNNER_EXECUTING){

				std::vector<int> completed;
			
				for (GitJob &job : jobs) {
					std::cout<<job.status<<job.job_type<<std::endl;
					if (job.status == JOB_COMPLETE){
						
						std::thread &thread = threads[job.job_idx];

						if (thread.joinable()){
							thread.join();
						}

						completed.push_back(job.job_idx);

					}
				}

				std::cout<<completed.size()<<std::endl;
				std::this_thread::sleep_for(std::chrono::seconds(1));

				if (jobs.size() < 1){
					runner_status = RUNNER_IDLE;
				}

				std::cout<<"RUNNING! "<<runner_status<<" "<<jobs.size()<<std::endl;

			}
		});
		
		// auto watcher = std::thread([&](){

		
		// });

		
		// watch_thread.join();
	}

	std::cout<<jobs.size()<<std::endl;


}

void Git::pause(){

	if (watch_thread.joinable()){
		watch_thread.join();
	}

}
