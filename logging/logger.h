#include <quill/Quill.h>
#include <string>
#include <sstream>


namespace utilities {
    namespace logging {
        class LoggerFactory {
            public:
                LoggerFactory(){
                    quill::Handler* stdout_handler = quill::stdout_handler();
                    stdout_handler->set_pattern("%(ascii_time) [%(process)] [%(thread)] %(logger_name) - %(message)", // format
                                            "%Y-%m-%d %H:%M:%S.%Qms",  // timestamp format
                                            quill::Timezone::GmtTime); 

                    quill::Handler* stderr_handler = quill::stderr_handler();
                    stderr_handler->set_pattern("%(ascii_time) [%(process)] [%(thread)] %(logger_name) - %(message)", // format
                                            "%Y-%m-%d %H:%M:%S.%Qms",  // timestamp format
                                            quill::Timezone::GmtTime);

                    static_cast<quill::ConsoleHandler*>(stdout_handler)->enable_console_colours();
                    static_cast<quill::ConsoleHandler*>(stderr_handler)->enable_console_colours(); 

                    config.default_handlers.emplace_back(stdout_handler);
                    config.default_handlers.emplace_back(stderr_handler);
                    
                    
                    quill::configure(config);
                    quill::start();


                }
                quill::Logger *getLogger(){

                    auto logger = quill::get_logger();
                    logger->set_log_level(level);
                        
                    return logger;

                };


            private:
                quill::Config config;
                quill::LogLevel level = quill::LogLevel::Info;
        };
    }
}