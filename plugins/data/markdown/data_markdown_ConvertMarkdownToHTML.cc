/**
 *
 *  data_markdown_ConvertMarkdownToHTML.cc
 *
 */

#include "data_markdown_ConvertMarkdownToHTML.h"

using namespace drogon;
using namespace drogon_model;
using namespace data::markdown;

void ConvertMarkdownToHTML::initAndStart(const Json::Value &config)
{


    logger_factory = utilities::logging::LoggerFactory();
    logger = logger_factory.createConsoleLogger("console");
    file_logger = logger_factory.createFileLogger("markdown_job", "blog.markdown.job.log");

    cache = utilities::cache::QuickStore<std::string, std::pair<data::markdown::CacheAction, std::string>>(100);


    db = drogon::app().getDbClient();
    tags_mapper = drogon::orm::Mapper<drogon_model::sqlite3::Tags>(db);
    categories_mapper = drogon::orm::Mapper<drogon_model::sqlite3::Categories>(db);



    maddy_config = std::make_shared<maddy::ParserConfig>();
    maddy_config->isEmphasizedParserEnabled = true; // default
    maddy_config->isHTMLWrappedInParagraph = true; // default

    parser = std::make_shared<maddy::Parser>(maddy_config);
    posts_mapper = drogon::orm::Mapper<drogon_model::sqlite3::Posts>(db);

    repo_path = config["repo_path"].asString();
    if (repo_path.size() == 0){
         auto config_path_env = getenv("REPO_PATH");
        if (config_path_env == NULL){

            auto current_working_directory = std::filesystem::current_path().string();
            std::stringstream repo_filepath;
            repo_filepath << current_working_directory << "/posts";

            utilities::filesystem::create_file_at_path(repo_filepath.str());

        } else {
            repo_path = std::string((char*)config_path_env);
        }

    }


    LOG_DEBUG(logger, "Markdown -> HTML converter job: Targeting repository path {}", repo_path);
    LOG_INFO(file_logger, "Markdown -> HTML converter job: Targeting repository path {}", repo_path);

    run_job = true;

    markdown_job = std::thread([&](){

        int article_process_interval = 60;
        auto process_interval = getenv("ARTICLE_PROCESS_INTERVAL");

        if (process_interval != NULL){
            article_process_interval = std::stoi((char*)process_interval);

        } else if (!config["process_interval"].isNull()) {
            article_process_interval = config["process_interval"].asInt();
        }

        LOG_INFO(logger, "Started Markdown -> HTML converter job.");
        LOG_DEBUG(file_logger, "Markdown -> HTML converter job: Running on process: {}", getpid());

        LOG_INFO(logger, "Started Markdown -> HTML converter job.");
        LOG_INFO(file_logger, "Markdown -> HTML converter job: Running on process: {}", getpid());

        int start_offset = rand() % article_process_interval + article_process_interval;

        LOG_DEBUG(logger, "Markdown -> HTML converter job: Starting in: {} seconds.", start_offset);
        LOG_DEBUG(file_logger, "Markdown -> HTML converter job: Starting in: {} seconds.", start_offset);

        std::unique_lock<std::mutex> lock(runner_mutex);
        runner_conditional.wait_for(lock, std::chrono::seconds(start_offset));


        while(run_job){

            try {

                findPosts(repo_path);
                loadPosts();
                savePosts();
                findTagsAndCategories();
                loadTagsAndCategories();
                saveTags();
                saveCategories();
                
            } catch(std::exception &e){
                LOG_CRITICAL(logger, "Markdown -> HTML converter job: Encountered critical error: {}. Restarting.", e.what());
                LOG_CRITICAL(file_logger, "Markdown -> HTML converter job: Encountered critical error: {}. Restarting.", e.what());
            }

            runner_conditional.wait_for(
                lock, 
                std::chrono::duration(
                    std::chrono::seconds(article_process_interval)
                ),  
                [&](){
                    return !run_job;
                }
            );
        }

    });

}


void ConvertMarkdownToHTML::findPosts(const std::string &repo_path){

    std::string md_post_ext(".post.md");
    std::string html_post_ext(".post.html");

    int discovered_articles = 0;
        
    for (auto &file : std::filesystem::recursive_directory_iterator(repo_path))
    {

        auto filepath = file.path().string();

        if (cache.get(filepath) == std::nullopt){

            std::string file_type = file.path().extension();
            std::string stub = file.path().stem().string();
            std::string name = std::filesystem::path(stub).stem().string();

            auto record_count = posts_mapper.countFuture(
                drogon::orm::Criteria("name", drogon::orm::CompareOperator::EQ, name)
            ).get();

            auto filename = file.path().filename().string();


            auto is_markdown = filename.find(md_post_ext) != std::string::npos;
            auto is_html = filename.find(html_post_ext) != std::string::npos;
            auto is_valid_post_document = is_html || is_markdown;

            if (is_valid_post_document && record_count == 0){

                cache.store(
                    filepath,
                    std::pair(
                        CacheAction::LOAD_POST,
                        filepath
                    )
                );


                discovered_articles += 1;


            }

        }
        

    }

    LOG_DEBUG(logger, "Markdown -> HTML converter job: Discovered: {} new articles.", discovered_articles);
    LOG_INFO(file_logger, "Markdown -> HTML converter job: Discovered: {} new articles.", discovered_articles);

}


void ConvertMarkdownToHTML::loadPosts(){

    int cache_idx = 0;
    int loaded_count = 0;
    for (const auto &filepath: cache.iter()){

        auto cache_item = cache.get(filepath).value();

        if (cache_item.first == CacheAction::LOAD_POST){
            std::stringstream buffer;
            std::ifstream ifs(filepath);
            std::string post_body;

            auto path = std::filesystem::path(filepath);
            std::string file_type = path.extension();

            if ( !ifs.is_open() ) { 
                LOG_WARNING(logger, "Markdown -> HTML converter job: Failed to open article at: {}", filepath);  
                LOG_WARNING(file_logger, "Markdown -> HTML converter job: Failed to open article at: {}", filepath);   

            }
            else {

                buffer << ifs.rdbuf(); 
                
            }

            if (file_type == ".md"){

                post_body = parser->Parse(buffer);

            }  else {

                post_body = buffer.str();

            }

            cache.store(
                filepath,
                std::pair(
                    CacheAction::SAVE_POST,
                    post_body
                )
            );

            loaded_count += 1;

        }

        cache_idx += 1;

    }


    LOG_DEBUG(logger, "Markdown -> HTML converter job: Loaded: {} new articles.", loaded_count);
    LOG_INFO(file_logger, "Markdown -> HTML converter job: Loaded: {} new articles.", loaded_count);

}


void ConvertMarkdownToHTML::savePosts(){

    int save_count = 0;
    int cache_idx = 0;
    for (const auto &filepath : cache.iter()){

        auto cache_item = cache.get(filepath).value();

        if (cache_item.first == CacheAction::SAVE_POST){

            std::string post_body = cache_item.second;

            auto path = std::filesystem::path(filepath);
            std::string stub = path.stem().string();
            std::string name = std::filesystem::path(stub).stem().string();

            auto post_id = uuid::generate_uuid_v4();

            Json::Value post_data;
            post_data["post_id"] = post_id;
            post_data["name"] = name;
            post_data["body"] = post_body;
            
            auto post = drogon_model::sqlite3::Posts(post_data);
            posts_mapper.insertFuture(post).get();

            save_count += 1;

            cache.store(
                filepath,
                std::pair(
                    CacheAction::PROCESSED_POST,
                    post_id
                )
            );
        
        }

        cache_idx += 1;
        
    }

    LOG_DEBUG(logger, "Markdown -> HTML converter job: Saved: {} new articles.", save_count);
    LOG_INFO(file_logger, "Markdown -> HTML converter job: Saved: {} new articles.", save_count); 

}


void ConvertMarkdownToHTML::findTagsAndCategories(){

    auto tags_ext = std::string(".tags.csv");
    auto categories_ext = std::string(".categories.csv");

    std::vector<std::pair<std::string, std::string>> post_directories;

    for (const auto &filepath : cache.iter()){

        auto cache_item = cache.get(filepath).value();

        if (cache_item.first == CacheAction::PROCESSED_POST){

            std::string post_directory = std::filesystem::path(filepath).parent_path();

            post_directories.push_back(
                std::pair(
                    post_directory,
                    cache_item.second
                )
            );
            
        }
    }

    int metadata_files_discovered = 0; 
    for (const auto &post_directory : post_directories){

        for (auto &metadata_file : std::filesystem::recursive_directory_iterator(post_directory.first))
        {

            auto metadata_filename = metadata_file.path().filename().string();

            if (metadata_filename.find(tags_ext)  != std::string::npos || metadata_filename.find(categories_ext)  != std::string::npos){

                auto tags_filepath = metadata_file.path().string();
                utilities::cache::StoreResult result = cache.store(
                    tags_filepath, 
                    std::pair(
                        CacheAction::LOAD_METADATA,
                        post_directory.second
                    )
                );

                auto increment_amount = result == utilities::cache::StoreResult::CACHE_ITEM_CREATED ? 1 : 0;
                metadata_files_discovered += increment_amount;

            }
        }

    }

    LOG_DEBUG(logger, "Markdown -> HTML converter job: Discovered: {} new metadata files.", metadata_files_discovered);
    LOG_INFO(file_logger, "Markdown -> HTML converter job: Discovered: {} new metadata files.", metadata_files_discovered);
    
}


void ConvertMarkdownToHTML::loadTagsAndCategories(){


    auto tags_ext = std::string(".tags.csv");
    auto categories_ext = std::string(".categories.csv");

    int cache_idx = 0;
    int metadata_files_loaded = 0;

    for (const auto &filepath : cache.iter()){

        auto cache_item = cache.get(filepath).value();

        auto metadata_filename = std::filesystem::path(filepath).filename().string();


        auto cache_action = metadata_filename.find(tags_ext) != std::string::npos ? CacheAction::SAVE_TAGS : CacheAction::SAVE_CATEGORIES;

        if (cache_item.first == CacheAction::LOAD_METADATA){

            std::stringstream buffer;
            std::ifstream ifs(filepath);       // note no mode needed

            if ( !ifs.is_open() ) { 
                LOG_WARNING(logger, "Markdown -> HTML converter job: Failed to open tags file at: {}", filepath);  
                LOG_WARNING(file_logger, "Markdown -> HTML converter job: Failed to open tags file at: {}", filepath);   

            }
            else {
    
                buffer << ifs.rdbuf();

                utilities::cache::StoreResult result = cache.store(
                    buffer.str(),
                    std::pair(
                        cache_action,
                        cache_item.second
                    )
                );

                auto increment_amount = result == utilities::cache::StoreResult::CACHE_ITEM_CREATED ? 1 : 0;
                metadata_files_loaded += increment_amount;

                ifs.close();

            }

        }

        cache_idx += 1;

    }


    LOG_DEBUG(logger, "Markdown -> HTML converter job: Loaded: {} new metadata files.", metadata_files_loaded);
    LOG_INFO(file_logger, "Markdown -> HTML converter job: Loaded: {} new metadata files.", metadata_files_loaded);

}


void ConvertMarkdownToHTML::saveTags(){

    int cache_idx = 0;
    int tags_saved = 0;

    for (const auto &metadata_content : cache.iter()){

        auto cache_item = cache.get(metadata_content).value();
        if (cache_item.first == CacheAction::SAVE_TAGS){

            for (const auto &metadata_item : utilities::string::split(metadata_content, ',')){

                auto metadata_id = uuid::generate_uuid_v4();
                auto record_count = tags_mapper.countFuture(
                    drogon::orm::Criteria("name", drogon::orm::CompareOperator::EQ, metadata_item) &&
                    drogon::orm::Criteria("post", drogon::orm::CompareOperator::EQ, cache_item.second)
                ).get();

                if (record_count == 0){

                    Json::Value data;
                    data["tag_id"] = metadata_id;
                    data["name"] = metadata_item;
                    data["post"] = cache_item.second;

                    auto tag = drogon_model::sqlite3::Tags(data);
                    tags_mapper.insertFuture(tag).get();

                    tags_saved += 1;

                }

                cache.store(
                    metadata_item,
                    std::pair(
                        CacheAction::PROCESSED_TAGS,
                        std::string("OK")
                    )
                );

            }

        }

        cache_idx += 1;
    }

    LOG_DEBUG(logger, "Markdown -> HTML converter job: Saved: {} new tags.", tags_saved);
    LOG_INFO(file_logger, "Markdown -> HTML converter job: Saved: {} new tags.", tags_saved);

}


void ConvertMarkdownToHTML::saveCategories(){

    int cache_idx = 0;
    int categories_saved = 0;

    for (const auto &metadata_content : cache.iter()){

        auto cache_item = cache.get(metadata_content).value();
        if (cache_item.first == CacheAction::SAVE_CATEGORIES){

            for (const auto &metadata_item : utilities::string::split(metadata_content, ',')){

                auto metadata_id = uuid::generate_uuid_v4();
                auto record_count = categories_mapper.countFuture(
                    drogon::orm::Criteria("name", drogon::orm::CompareOperator::EQ, metadata_item) &&
                    drogon::orm::Criteria("post", drogon::orm::CompareOperator::EQ, cache_item.second)
                ).get();

                if (record_count == 0){

                    Json::Value data;
                    data["category_id"] = metadata_id;
                    data["name"] = metadata_item;
                    data["post"] = cache_item.second;

                    auto category = drogon_model::sqlite3::Categories(data);
                    categories_mapper.insertFuture(category).get();

                    categories_saved += 1;

                }

                cache.store(
                    metadata_item,
                    std::pair(
                        CacheAction::PROCESSED_CATEGORIES,
                        std::string("OK")
                    )
                );


            }

        }

        cache_idx += 1;

    }

    LOG_DEBUG(logger, "Markdown -> HTML converter job: Saves: {} new categories.", categories_saved);
    LOG_INFO(file_logger, "Markdown -> HTML converter job: Saved: {} new categories.", categories_saved);

}


void ConvertMarkdownToHTML::shutdown(){
    /// Shutdown the plugin
    auto logger_factory = utilities::logging::LoggerFactory();
    auto logger = logger_factory.createConsoleLogger("console");
    auto file_logger = logger_factory.createFileLogger("markdown_job", "blog.markdown.job.log");
    
    run_job = false;
    runner_conditional.notify_all();

    LOG_INFO(logger, "Markdown -> HTML Converter job has been notified of shutdown. Please wait...");
    LOG_INFO(file_logger, "Markdown -> HTML Converter job has been notified of shutdown. Please wait...");

    if(markdown_job.joinable()){
        markdown_job.join();
    }

    LOG_INFO(logger, "Markdown -> HTML Converter job has stopped.");
    LOG_INFO(file_logger, "Markdown -> HTML Converter job has stopped.");
}
