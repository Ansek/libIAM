cmake_minimum_required(VERSION 3.15)
project(NSA_RS
    VERSION 0.1
    DESCRIPTION "Negative Selection Algorithm based on Random Strings"
    LANGUAGES C)

string(COMPARE EQUAL "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}" is_top_level)

if(is_top_level)
    find_package(IAM REQUIRED)
endif()

add_library(NSA_RS SHARED)
target_link_libraries(NSA_RS PUBLIC IAM)

set(sources
    nsa_rs.c)

target_sources(NSA_RS PRIVATE ${sources})

install(TARGETS NSA_RS DESTINATION lib/iam_plugins)