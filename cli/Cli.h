#include <drogon/drogon.h>

#ifndef LOGGER_FACTORY
#define LOGGER_FACTORY
#include <utilities/logging/logger_factory.h>
#endif

#include <coroutine>
#include <chrono>
#include <unistd.h>
#include <cstdlib>
#include <filesystem>
#include <cli/CLI11.hpp>
#include <auth/bcrypt/bcrypt.h>
#include <plugins/global/cors/Cors.h>


#include "CLI11.hpp"

#ifndef CACHE
#define CACHE
#include "utilities/storage/quick_store.h"
#endif

#include "Config.h"


namespace cli {

    class Cli {
        public:
            Cli():
                app("Blog Server")
            {   
                
                quill::start();

                std::stringstream config_filepath;
                config_filepath << current_working_directory << "/config.json";

                api_config_path = config_filepath.str();

                setup_subcommand = app.add_subcommand("setup", "Setup JWT token for the server.");
                run_subcommand = app.add_subcommand("run", "Run the server.");
                app.require_subcommand(1);


                setup_subcommand->add_option("-s, --secret", api_secret, "Secret word used to generate API key")->required(true);
                run_subcommand->add_option("-p, --port", api_port, "Port for server to run on.");
                run_subcommand->add_option("-c, --config", api_config_path, "Path to config JSON file for server.");

                logger = logger_factory.createConsoleLogger("console");
                file_logger = logger_factory.createFileLogger("main", "blog.main.log");
                
            }

            int execute(int argc, char** argv){
                
                CLI11_PARSE(app, argc, argv);

                if (run_subcommand->count() > 0){

                    return run();   

                } else if (setup_subcommand->count() > 0){

                    return setup();

                }

                return 1;
            
            }

        private:
            CLI::App app;
            CLI::App *setup_subcommand;
            CLI::App *run_subcommand;
            std::string api_secret;
            int api_port = 8122;
            std::string current_working_directory = std::filesystem::current_path().string();
            std::string api_config_path;
            utilities::logging::LoggerFactory logger_factory;
            quill::Logger* logger;
            quill::Logger* file_logger;

            int run(){

                auto config_env_path = getenv("DROGON_CONFIG_PATH");
                if (config_env_path) {
                    api_config_path = std::string(config_env_path);
                }

        
                auto server_port = getenv("SERVER_PORT");
                if (server_port) {
                    api_port = std::stoi(std::string(server_port));
                }

                drogon::app().addListener("0.0.0.0", api_port);
                drogon::app().loadConfigFile(api_config_path);

                LOG_INFO(logger,"Config successfully loaded from: {}", api_config_path);
                LOG_INFO(file_logger,"Config successfully loaded from: {}", api_config_path);


                const std::vector<CORSOption> options {
                    CORSOption(
                        std::string("Access-Control-Allow-Origin"),
                        std::vector<std::string>{"*"}
                    ),
                    CORSOption(
                        std::string("Access-Control-Request-Method"),
                        std::vector<std::string>{
                            "GET",
                            "HEAD",
                            "OPTIONS"
                        }
                    )
                };

                drogon::app().registerHttpResponseCreationAdvice([&](
                    const HttpResponsePtr &response
                ){

                    auto cors_plugin = drogon::app().getPlugin<app::global::Cors>();
                    cors_plugin->setHeaders(options, response);
                    
                });


                LOG_INFO(logger, "Serving on port: {}\n", api_port);
                LOG_INFO(file_logger, "Serving on port: {}\n", api_port);

                drogon::app().run();

                return 0;

            }

            int setup(){

                auto api_env_secret = getenv("BLOG_API_KEY");
                if (api_env_secret) {
                    api_secret = api_env_secret;
                }
                
                std::string encrypted_api_secret = bcrypt::encrypt(std::string(api_secret));

                assert(bcrypt::matches(api_secret, encrypted_api_secret));

                std::stringstream dotenv_filepath;
                dotenv_filepath << current_working_directory << "/.env";

                std::string dotenv_path = dotenv_filepath.str();

                std::ofstream dotenv_file(dotenv_path);
                if (!dotenv_file) {

                    LOG_CRITICAL(logger, "Err. - could not create .env file at {}", dotenv_path);
                    LOG_CRITICAL(file_logger, "Err. - could not create .env file at {}", dotenv_path);

                    return 1;
                }

                std::stringstream secret_env_key;
                secret_env_key << "BLOG_API_SECRET="<<encrypted_api_secret;

                dotenv_file.write(
                    secret_env_key.str().c_str(), 
                    secret_env_key.str().size()
                );

                dotenv_file.close();

                return 0;

            }
    };

}