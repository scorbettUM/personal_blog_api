#include <thread>
#include <string>
#include <sstream>
#include "command.h"
#include <vector>
#include <variant>
#include <iostream>


namespace manager {
    enum OperationState {
        JOB_IDLE,
        JOB_RUNNING,
        JOB_COMPLETE,	
    };

    enum JobTypes {
        CHECK_IF_REPO,
        INIT_REPO,
        CLONE_REPO,
        PULL_REPO,
        CHECKOUT_REPO,
        BRANCH_REPO,
        FETCH_REPO
    };

    class GitJob {
        public:
            GitJob(
                JobTypes git_job_type,
                uint64_t job_id
            ){
                job_type = git_job_type;
                job_idx = job_id;
            }
            uint64_t job_idx;
            JobTypes job_type;
            int command_status;
            OperationState status = JOB_IDLE;
            template<typename... Args>uint64_t run(Args... args) {

                status = JOB_RUNNING;

                std::vector<std::string> command_partials = {args...};

                command_string = std::ostringstream();

                for (std::string arg : command_partials){
                    command_string << arg;
                }

                std::cout<<command_string.str()<<std::endl;

                auto command = Command();
                auto result = command.exec(command_string.str());



                command_status = result.exit_status;

                status = JOB_COMPLETE;

                return job_idx;

            };
       
        private:
            std::ostringstream command_string;
            

        private:
            std::thread job;
    };

}