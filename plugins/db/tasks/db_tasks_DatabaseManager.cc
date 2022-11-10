/**
 *
 *  db_tasks_DatabaseManager.cc
 *
 */

#include "db_tasks_DatabaseManager.h"


using namespace drogon;
using namespace db::tasks;

void DatabaseManager::initAndStart(const Json::Value &config)
{
    /// Initialize and start the plugin
    auto db = drogon::app().getDbClient();

    auto logger_factory = utilities::logging::LoggerFactory();
    auto logger = logger_factory.createConsoleLogger("console");
    auto file_logger = logger_factory.createFileLogger("database_tasks", "blog.database.tasks.log");


    auto current_working_directory = std::filesystem::current_path().string();
    std::stringstream database_filepath;
    database_filepath << current_working_directory << "/db/blog.db";

    auto api_database_path = database_filepath.str();
    auto database_path = getenv("SQLITE_DATABASE_PATH");
    if (database_path != NULL) {
        api_database_path = std::string(database_path);
        
    } else if (!config["database_path"].isNull()){
        api_database_path = config["database_path"].asString();

    }

    utilities::filesystem::create_file_at_path(api_database_path);
    
    LOG_INFO(logger, "Database Manager connecting to database at: {}", api_database_path);
    LOG_INFO(file_logger, "Database Manager connecting to database at: {}", api_database_path);

    if (std::ifstream(api_database_path)){

        LOG_INFO(logger, "Database at: {} exists. Skipping creation.", api_database_path);
        LOG_INFO(file_logger, "Database at: {} exists. Skipping creation.", api_database_path);

    } else {
        std::ofstream database_file(api_database_path);
        if (!database_file) {

            LOG_CRITICAL(logger, "Err. - could not create or connect database file at {}", api_database_path);
            LOG_CRITICAL(file_logger, "Err. - could not create or connect database file at {}", api_database_path);
        }
    }

    TableTypes app_tables;

    for (const auto &table : app_tables.tables){

        db->execSqlAsync(table.second, [&](const drogon::orm::Result &result) {

            assert(result.size() == 0);


        }, [&](const drogon::orm::DrogonDbException &db_error) {
            const auto exception = db_error.base();

            LOG_CRITICAL(logger, "Encountered error creating or connecting to Posts table: {}", exception.what());
            LOG_CRITICAL(file_logger, "Encountered error creating or connecting to Posts table: {}", exception.what());

        });

        LOG_INFO(logger, "Successfully created or connected to {} table.", table.first);
        LOG_INFO(file_logger, "Successfully created or connected to {} table.", table.first);
    }
    


}

void DatabaseManager::shutdown() 
{
    /// Shutdown the plugin
}
