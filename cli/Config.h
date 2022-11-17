#include <string>

namespace cli {
    class Config {
        public:
            Config(
                int port,
                std::string config_filepath
            ){
                api_port = port;
                api_config_filepath = config_filepath;
            }
            
            int api_port;
            std::string api_config_filepath;
    };
}