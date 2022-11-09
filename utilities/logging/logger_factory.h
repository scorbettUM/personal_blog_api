#include <quill/Quill.h>
#include <string>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <map>


namespace utilities {
    namespace logging {
        class LoggerFactory {
            public:
                LoggerFactory(){

                    auto env_log_level = getenv("BLOG_LOG_LEVEL");
                    if (env_log_level){

                        auto app_log_level = std::string(env_log_level);
                        if(log_level_map.count(app_log_level) > 0){
                            
                            log_level = log_level_map[app_log_level];

                        }
                    }

                    auto current_working_directory = std::filesystem::current_path().string();
                    logs_output_directory = current_working_directory;
                    auto log_dir = getenv("BLOG_API_LOGGING_DIR");
                    if (log_dir){
                        logs_output_directory = std::string(log_dir);
                    } 

                }

                template<typename T>quill::Logger* createConsoleLogger(
                    T logger_name
                ){

                    console_handler = quill::stdout_handler();
                    console_handler->set_pattern(
                        "%(ascii_time) [%(process)] [%(thread)] %(logger_name) - %(message)", // format
                        "%Y-%m-%d %H:%M:%S.%Qms",  // timestamp format
                        quill::Timezone::GmtTime
                    ); 

                    static_cast<quill::ConsoleHandler*>(console_handler)->enable_console_colours();

                    auto logger_store_name = std::string(logger_name);
                    auto logger = quill::create_logger(logger_name, console_handler);
                    logger->set_log_level(log_level);

                    return logger;
                }

                template<typename T, typename K>quill::Logger* createFileLogger(
                    T logger_name,
                    K filename
                ){

                    auto logs_file_path = createLogFile(std::string(filename));

                    file_config = quill::time_rotating_file_handler(
                        logs_file_path, 
                        "a", 
                        "daily", 
                        1, 
                        10, 
                        quill::Timezone::GmtTime, 
                        "00:00"
                    );
                    file_config->set_pattern(
                        "%(ascii_time) [%(process)] [%(thread)] %(logger_name) - %(message)", // format
                        "%Y-%m-%d %H:%M:%S.%Qms",  // timestamp format
                        quill::Timezone::GmtTime
                    ); 

                    auto logger = quill::create_logger(logger_name, file_config);
                    logger->set_log_level(log_level);

                    return logger;
                }

            private:
                quill::Handler* console_handler;
                quill::Handler* file_config;
                quill::LogLevel log_level = quill::LogLevel::Info;
                std::string logs_output_directory;
                std::map<std::string, quill::LogLevel> log_level_map = {
                    {"debug", quill::LogLevel::Debug},
                    {"info", quill::LogLevel::Info},
                    {"warning", quill::LogLevel::Warning},
                    {"error", quill::LogLevel::Error},
                    {"critical", quill::LogLevel::Critical},
                    {"trace_l1", quill::LogLevel::TraceL1},
                    {"trace_l2", quill::LogLevel::TraceL2},
                    {"trace_l3", quill::LogLevel::TraceL3},
                    {"backtrace", quill::LogLevel::Backtrace}
                };


                std::string createLogFile(
                    std::string log_file
                ){ 

                    std::stringstream logs_path;
                    logs_path << logs_output_directory << "/" << log_file;

                    std::string logs_file_path = logs_path.str();

                    std::error_code err;


                    if (!std::filesystem::exists(logs_output_directory)){   
                        std::filesystem::create_directories(logs_output_directory, err);
                    }

                    if (!std::filesystem::exists(logs_file_path)){
                        auto log_file = std::ofstream(logs_file_path);
                        log_file.close();
                    }
                    

                    return logs_file_path;
                }
        };
    }
}