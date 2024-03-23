cmake_minimum_required(VERSION 3.15)
project(NSA_RA
    VERSION 0.1
    DESCRIPTION "Negative Selection Algorithm based on Rectangular Areas"
    LANGUAGES C)

string(COMPARE EQUAL "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}" is_top_level)

if(is_top_level)
    find_package(IAM REQUIRED)
endif()

add_library(NSA_RA SHARED)
target_link_libraries(NSA_RA PUBLIC IAM)
    
set(sources
    nsa_ra.c)

target_sources(NSA_RA PRIVATE ${sources})

install(TARGETS NSA_RA DESTINATION lib/iam_plugins)