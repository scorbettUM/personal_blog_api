#pragma once
#include <filesystem>
#include <fstream>
#include <string>


namespace utilities {
    namespace filesystem {

        inline void create_file_at_path(std::string filepath){
            std::error_code err;
            auto database_filepath = std::filesystem::path(filepath);
            if (!std::filesystem::exists(database_filepath.parent_path())){   
                std::filesystem::create_directories(database_filepath, err);
            }

            if (!std::filesystem::exists(database_filepath)){
                auto log_file = std::ofstream(database_filepath);
                log_file.close();
            }
        }
    }
}