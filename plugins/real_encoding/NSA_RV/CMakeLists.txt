cmake_minimum_required(VERSION 3.15)
project(NSA_RV
    VERSION 0.1
    DESCRIPTION "Negative Selection Algorithm based on Real-Valued"
    LANGUAGES C)

string(COMPARE EQUAL "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}" is_top_level)

if(is_top_level)
    find_package(IAM REQUIRED)
endif()

add_library(NSA_RV SHARED)
target_link_libraries(NSA_RV PUBLIC IAM)

set(sources
    nsa_rv.c)

target_sources(NSA_RV PRIVATE ${sources})

install(TARGETS NSA_RV DESTINATION lib/iam_plugins)