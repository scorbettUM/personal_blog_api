#pragma once
#include <thread>
#include <string>
#include <sstream>
#include "command.h"
#include <vector>
#include <variant>
#include <iostream>


namespace task {
   
    namespace types {
        enum OperationState {
            COMMAND_IDLE,
            COMMAND_RUNNING,
            COMMAND_COMPLETE,	
            COMMAND_TERMINATED
        };

        enum ConnandTypes {
            CHECK_IF_REPO,
            INIT_REPO,
            CLONE_REPO,
            PULL_REPO,
            CHECKOUT_REPO,
            BRANCH_REPO,
            FETCH_REPO
        };

        class GitCommand {
            public:
                GitCommand(
                    ConnandTypes git_command_type,
                    uint64_t command_id
                ){
                    command_type = git_command_type;
                    commmand_idx = command_id;
                }
                uint64_t commmand_idx;
                ConnandTypes command_type;
                int command_status;
                OperationState status = COMMAND_IDLE;
                template<typename... Args>uint64_t run(Args... args) {

                    status = COMMAND_IDLE;

                    std::vector<std::string> command_partials = {args...};

                    command_string = std::ostringstream();

                    for (std::string arg : command_partials){
                        command_string << arg;
                    }

                    auto command = Command();
                    auto result = command.exec(command_string.str());

                    command_status = result.exit_status;

                    status = COMMAND_COMPLETE;

                    return commmand_idx;

                }
        
            private:
                std::ostringstream command_string;
 
        };
    }

}