cmake_minimum_required(VERSION 3.15)
project(IAM
    VERSION 0.1
    DESCRIPTION "Immune Algorithm Manager Library"
    LANGUAGES C)

option(IAM_INCLUDE_PLUGINS "Build or install IAM plugins" OFF)
option(IAM_BUILD_EXAMPLES "Build IAM examples" OFF)

if (NOT DEFINED IAM_PLUGINS_DIR)
    set(IAM_PLUGINS_DIR{PATH} "plugins")
endif()

add_library(IAM SHARED)
add_library(IAM::IAM ALIAS IAM)

target_compile_definitions(IAM PRIVATE "IAM_PLUGINS_DIR=\"${IAM_PLUGINS_DIR}\"")

set(public_headers
    include/iam/iam.h
    include/iam/init.h
    include/iam/logger.h
    include/iam/plugin.h)
set(sources
    ${public_headers}
    src/os/os.h
    src/iam.c
    src/list.h
    src/list.c
    src/logger.c
    src/plugin_manager.h
    src/plugin_manager.c
    src/version.h)

if (WIN32)
    list(APPEND sources src/os/win.h src/os/win.c)
else()
    list(APPEND sources src/os/unix.h src/os/unix.c)
endif()

target_include_directories(IAM
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src")
    
target_sources(IAM PRIVATE ${sources})

include(CMakePackageConfigHelpers)
configure_package_config_file(cmake/IAMConfig.cmake.in IAMConfig.cmake
    INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/iam")
write_basic_package_version_file(IAMConfigVersion.cmake
    VERSION 0.0.1
    COMPATIBILITY SameMajorVersion)

install(TARGETS IAM
    EXPORT IAMTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(TARGETS IAM DESTINATION lib)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/IAMConfig.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/IAMConfigVersion.cmake
        DESTINATION lib/cmake/iam )
install(EXPORT IAMTargets
        FILE "IAMTargets.cmake"
        NAMESPACE IAM::
        DESTINATION lib/cmake/iam
)    
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/iam"
        DESTINATION include
)

if (IAM_INCLUDE_PLUGINS)
    add_subdirectory(plugins)
endif()

if (IAM_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()