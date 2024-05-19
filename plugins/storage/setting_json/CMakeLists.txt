cmake_minimum_required(VERSION 3.15)
project(setting_json
    VERSION 0.1
    DESCRIPTION "Saving settings in a json file"
    LANGUAGES C)

string(COMPARE EQUAL "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}" is_top_level)

if(is_top_level)
    find_package(IAM REQUIRED)
endif()

include(FetchContent)
FetchContent_Declare(
    jansson
    GIT_REPOSITORY https://github.com/akheron/jansson.git
    GIT_TAG v2.14
    GIT_SHALLOW FALSE
)
set(JANSSON_WITHOUT_TESTS ON)
set(USE_WINDOWS_CRYPTOAPI OFF)
set(JANSSON_EXAMPLES OFF)
set(JANSSON_BUILD_DOCS OFF)
set(JANSSON_BUILD_MAN OFF)
FetchContent_MakeAvailable(jansson)

add_library(setting_json SHARED)
target_link_libraries(setting_json PUBLIC IAM jansson)
target_include_directories(setting_json PRIVATE ${jansson_BINARY_DIR}/include)

set(sources
    setting_json.c)

target_sources(setting_json PRIVATE ${sources})

install(TARGETS setting_json DESTINATION lib/iam_plugins)