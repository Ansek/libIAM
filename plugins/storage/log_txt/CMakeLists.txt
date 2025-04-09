cmake_minimum_required(VERSION 3.15)
project(log_txt
    VERSION 0.1
    DESCRIPTION "Saving logs in a txt file"
    LANGUAGES C)

string(COMPARE EQUAL "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}" is_top_level)

if(is_top_level)
    find_package(IAM REQUIRED)
endif()

add_library(log_txt SHARED)
target_link_libraries(log_txt PUBLIC IAM)

set(sources
    log_txt.c)

target_sources(log_txt PRIVATE ${sources})

install(TARGETS log_txt DESTINATION lib/iam_plugins)