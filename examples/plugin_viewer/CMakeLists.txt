cmake_minimum_required(VERSION 3.15)
project(plugin_viewer
    VERSION 1.0
    DESCRIPTION "Displays information about detected plugins."
    LANGUAGES C)

string(COMPARE EQUAL "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}" is_top_level)

if(is_top_level)
    find_package(IAM REQUIRED)
endif()
 
add_executable(plugin_viewer viewer.c)
target_link_libraries(plugin_viewer PRIVATE IAM::IAM)