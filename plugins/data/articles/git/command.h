#include <string>
#include <array>
#include <iostream>

#ifndef LOGGER_FACTORY
#define LOGGER_FACTORY
#include <utilities/logging/logger_factory.h>
#endif



namespace manager {
    class CommandResult {

        public:
            CommandResult(
                std::string command_output,
                int exit_code
            ){
                output = command_output;
                exit_status = exit_code;
            };

            std::string output;
            int exit_status;

            friend std::ostream &operator<<(std::ostream &os, const CommandResult &result) {
                os << "command exitstatus: " << result.exit_status << " output: " << result.output;
                return os;
            }
            bool operator==(const CommandResult &rhs) const {
                return output == rhs.output &&
                    exit_status == rhs.exit_status;
            }
            bool operator!=(const CommandResult &rhs) const {
                return !(rhs == *this);
            }
    };

    class Command {

        public:
            /**
             * Execute system command and get STDOUT result.
             * Like system() but gives back exit status and stdout.
             * @param command system command to execute
             * @return CommandResult containing STDOUT (not stderr) output & exitstatus
             * of command. Empty if command failed (or has no output). If you want stderr,
             * use shell redirection (2&>1).
             */

            Command(
            ){

                auto logger_factory = utilities::logging::LoggerFactory();
                logger = logger_factory.createConsoleLogger("console");
                file_logger = logger_factory.createFileLogger("articles_job", "blog.articles.job.log");
            }

            std::string command;
   
            CommandResult exec(const std::string &shell_command) {

                LOG_DEBUG(logger,"Executing shell command: {}", shell_command);
                LOG_DEBUG(file_logger,"Executing shell command: {}", shell_command);

                int exitcode = 255;
                std::array<char, 1048576> buffer {};
                std::string result;
        #ifdef _WIN32
        #define popen _popen
        #define pclose _pclose
        #define WEXITSTATUS
        #endif
                FILE *pipe = popen(shell_command.c_str(), "r");
                if (pipe == nullptr) {
                    throw std::runtime_error("popen() failed!");
                }
                try {
                    std::size_t bytesread;
                    while ((bytesread = fread(buffer.data(), sizeof(buffer.at(0)), sizeof(buffer), pipe)) != 0) {
                        result += std::string(buffer.data(), bytesread);
                    }
                } catch (...) {
                    exitcode = WEXITSTATUS(pclose(pipe));
                    throw;
                }
                exitcode = WEXITSTATUS(pclose(pipe));

                LOG_DEBUG(logger, "Command returned with exit code: {}", exitcode);
                LOG_DEBUG(file_logger, "Command returned with exit code: {}", exitcode);

                return CommandResult{result, exitcode};
            }
        private:
            quill::Logger *logger;
            quill::Logger *file_logger;
        };
}