cmake_minimum_required(VERSION 3.5)
project(personal_blog_api CXX)
include(CheckIncludeFileCXX)



check_include_file_cxx(any HAS_ANY)
check_include_file_cxx(string_view HAS_STRING_VIEW)
check_include_file_cxx(coroutine HAS_COROUTINE)
if (NOT "${CMAKE_CXX_STANDARD}" STREQUAL "")
    # Do nothing
elseif (HAS_ANY AND HAS_STRING_VIEW AND HAS_COROUTINE)
    set(CMAKE_CXX_STANDARD 20)
elseif (HAS_ANY AND HAS_STRING_VIEW)
    set(CMAKE_CXX_STANDARD 17)
else ()
    set(CMAKE_CXX_STANDARD 14)
endif ()

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(${PROJECT_NAME} main.cc)

# ##############################################################################
# If you include the drogon source code locally in your project, use this method
# to add drogon 
# add_subdirectory(drogon) 
# target_link_libraries(${PROJECT_NAME} PRIVATE drogon)
#
# and comment out the following lines
find_package(Drogon CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE Drogon::Drogon)

find_package(quill REQUIRED)

# Linking your project against quill
target_link_libraries(${PROJECT_NAME} PRIVATE quill::quill)

add_subdirectory(utilities/env/dotenv)
target_link_libraries(${PROJECT_NAME} PRIVATE cpp_dotenv)

add_library(bcrypt auth/bcrypt/bcrypt.cpp auth/bcrypt/blowfish.cpp)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-fcoroutines -O3 -std=c++20")

# ##############################################################################

if (CMAKE_CXX_STANDARD LESS 17)
    # With C++14, use boost to support any, string_view and filesystem
    message(STATUS "use c++14")
    find_package(Boost 1.61.0 REQUIRED)
    target_link_libraries(${PROJECT_NAME} PUBLIC Boost::boost)
elseif (CMAKE_CXX_STANDARD LESS 20)
    message(STATUS "use c++17")
else ()
    message(STATUS "use c++20")
endif ()


aux_source_directory(cli CLI_SRC)
aux_source_directory(controllers CTL_SRC)
aux_source_directory(filters FILTER_SRC)
aux_source_directory(plugins PLUGIN_SRC)
aux_source_directory(models MODEL_SRC)
aux_source_directory(auth/bcrypt BCRYPT_SRC)
aux_source_directory(auth/jwt-cpp JWT_CPP_SRC)
aux_source_directory(auth/picojson PICOJSON_SRC)
aux_source_directory(utilities/logging UTILS_LOGGING_SRC)
aux_source_directory(utilities/filesystem UTILS_FILESYSTEM_SRC)
aux_source_directory(utilities/storage UTILS_STORAGE_SRC)
aux_source_directory(controllers/api/v1/Posts API_V1_POSTS_SRC)
aux_source_directory(controllers/api/v1/Tags API_V1_TAGS_SRC)
aux_source_directory(controllers/api/v1/Categories API_V1_CATEGORIES_SRC)
aux_source_directory(plugins/db/tasks PLUGIN_DB_TASKS_SRC)
aux_source_directory(plugins/auth/jwt PLUGIN_AUTH_JWT_SRC)
aux_source_directory(plugins/global/cors PLUGIN_GLOBAL_CORS_SRC)
aux_source_directory(plugins/tasks PLUGIN_TASKS_SRC)
aux_source_directory(plugins/tasks/types/post PLUGIN_POST_TASKS_SRC)


drogon_create_views(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/views
                    ${CMAKE_CURRENT_BINARY_DIR})
# use the following line to create views with namespaces.
# drogon_create_views(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/views
#                     ${CMAKE_CURRENT_BINARY_DIR} TRUE)
# use the following line to create views with namespace CHANGE_ME prefixed
# and path namespaces.
# drogon_create_views(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/views
#                     ${CMAKE_CURRENT_BINARY_DIR} TRUE CHANGE_ME)

target_include_directories(${PROJECT_NAME}
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
                                   ${CMAKE_CURRENT_SOURCE_DIR}/models
                                   ${CMAKE_CURRENT_SOURCE_DIR}/auth/bcrypt/
                            )
target_sources(
    ${PROJECT_NAME}
    PRIVATE
    ${SRC_DIR}
    ${CLI_SRC}
    ${CTL_SRC}
    ${FILTER_SRC}
    ${PLUGIN_SRC}
    ${MODEL_SRC}
    ${PICOJSON_SRC}
    ${JWT_CPP_SRC}
    ${API_V1_POSTS_SRC}
    ${API_V1_TAGS_SRC}
    ${API_V1_CATEGORIES_SRC}
    ${VIEWS_V1_APP_SRC}
    ${PLUGIN_DB_TASKS_SRC}
    ${PLUGIN_GLOBAL_CORS_SRC}
    ${UTILS_LOGGING_SRC}
    ${UTILS_FILESYSTEM_SRC}
    ${UTILS_STORAGE_SRC}
    ${PLUGIN_TASKS_SRC}
    ${PLUGIN_POST_TASKS_SRC}
    ${PLUGIN_AUTH_JWT_SRC}
    ${BCRYPT_SRC}
)
# ##############################################################################
# uncomment the following line for dynamically loading views 
# set_property(TARGET ${PROJECT_NAME} PROPERTY ENABLE_EXPORTS ON)

# ##############################################################################

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
add_subdirectory(test)
