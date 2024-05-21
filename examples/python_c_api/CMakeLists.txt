cmake_minimum_required(VERSION 3.15)
project(iam_py
    VERSION 1.0
    DESCRIPTION "Wrapper for use in Python."
    LANGUAGES C)

string(COMPARE EQUAL "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}" is_top_level)

if(is_top_level)
    find_package(IAM REQUIRED)
endif()

find_package(Python COMPONENTS Interpreter NumPy)



add_library(iam_py SHARED)
target_link_libraries(iam_py PRIVATE IAM Python::NumPy)

set_target_properties(iam_py
    PROPERTIES PREFIX "" OUTPUT_NAME "iam" SUFFIX ".pyd")

set(sources
    iam_py.c)

target_sources(iam_py PRIVATE ${sources})

install(TARGETS iam_py DESTINATION lib)