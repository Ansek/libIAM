cmake_minimum_required(VERSION 3.15)
project(IAM_Test
    DESCRIPTION "Test for Immune Algorithm Manager library"
    LANGUAGES C)

include(FetchContent)
FetchContent_Declare(
    unity
    GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
    GIT_TAG v2.6.0
    GIT_SHALLOW FALSE
)
FetchContent_MakeAvailable(unity)

FetchContent_Declare(
    fff
    GIT_REPOSITORY https://github.com/meekrosoft/fff.git
    GIT_TAG v1.1
    GIT_SHALLOW FALSE
)
FetchContent_MakeAvailable(fff)

set(base_mock_src
    mock/os/os.c
    mock/memory.c
    mock/mock.c)
set(list_mock_src
    ${base_mock_src}
    mock/list.c)
set(all_mock_src
    ${list_mock_src}
    mock/iam/logger.c)

set(dirs
    mock
    ../src
    ../include
    ${fff_SOURCE_DIR})

add_library(iam_mock_lib SHARED)
target_sources(iam_mock_lib PRIVATE ${all_mock_src})
target_include_directories(iam_mock_lib PRIVATE ${dirs})

macro(add_test_file name src lib)
    add_executable(iam_test_${name}_app test_${name}.c)
    target_sources(iam_test_${name}_app PRIVATE ${${src}})
    target_link_libraries(iam_test_${name}_app PRIVATE ${${lib}})
    target_include_directories(iam_test_${name}_app PRIVATE ${dirs})
    add_test(NAME iam_test_${name} COMMAND iam_test_${name}_app)
endmacro()

set(libs
    unity)
set(mock_libs
    ${libs}
    iam_mock_lib)
    
set(list_src
    ${base_mock_src}
    ../src/list.c)    
add_test_file(list list_src libs)

set(logger_src
    ${list_mock_src}
    ../src/logger_manager.c)
add_test_file(logger logger_src libs)

set(plugin_src
    ../src/plugin_manager.c)
add_test_file(plugin plugin_src mock_libs)