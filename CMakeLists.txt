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


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-fcoroutines -O3")


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


aux_source_directory(controllers CTL_SRC)
aux_source_directory(filters FILTER_SRC)
aux_source_directory(plugins PLUGIN_SRC)
aux_source_directory(models MODEL_SRC)
aux_source_directory(utilities/logging LOGGING_SRC)
aux_source_directory(utilities/filesystem FILESYSTEM_SRC)
aux_source_directory(controllers/api/v1/Posts API_V1_POSTS_SRC)
aux_source_directory(plugins/data/articles PLUGIN_DATA_ARTICLES_SRC)
aux_source_directory(plugins/data/articles/git GIT_PLUGIN_SRC)
aux_source_directory(plugins/data/markdown PLUGIN_DATA_MARKDOWN_SRC)
aux_source_directory(plugins/db/tasks PLUGIN_DB_TASKS_SRC)
aux_source_directory(plugins/global/cors PLUGIN_GLOBAL_CORS_SRC)


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
                                   ${CMAKE_CURRENT_SOURCE_DIR}/models)
target_sources(
    ${PROJECT_NAME}
    PRIVATE
    ${SRC_DIR}
    ${CTL_SRC}
    ${FILTER_SRC}
    ${PLUGIN_SRC}
    ${MODEL_SRC}
    ${API_V1_POSTS_SRC}
    ${VIEWS_V1_APP_SRC}
    ${PLUGIN_DATA_ARTICLES_SRC}
    ${PLUGIN_DATA_MARKDOWN_SRC}
    ${PLUGIN_DB_TASKS_SRC}
    ${PLUGIN_GLOBAL_CORS_SRC}
    ${GIT_PLUGIN_SRC}
    ${LOGGING_SRC}
    ${FILESYSTEM_SRC}
)
# ##############################################################################
# uncomment the following line for dynamically loading views 
# set_property(TARGET ${PROJECT_NAME} PROPERTY ENABLE_EXPORTS ON)

# ##############################################################################

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
add_subdirectory(test)
