cmake_minimum_required(VERSION 3.10)
project(blackhole VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


option(BUILD_TESTS "Build tests" ON)
option(BUILD_CREATOR "Build creator" ON)
option(ENABLE_COLORS "Enable colors" ON)

set(CONFIG_DIRECTORY "/etc/blackhole" CACHE STRING "The default configuration directory")
set(CONFIG_DOT_DIRECTORY "conf.d" CACHE STRING "The name of the conf.d directory")

set(EXECUTABLE_NAME bh)


find_package(CURL REQUIRED)


find_package(nlohmann_json 3.2.0 REQUIRED)

if (ENABLE_COLORS)
    add_compile_definitions(ENABLE_COLOR_OUTPUT)
endif()

add_compile_definitions(CONFIG_DIRECTORY="${CONFIG_DIRECTORY}")
add_compile_definitions(CONFIG_DOT_DIRECTORY="${CONFIG_DOT_DIRECTORY}")

set(SOURCES
    src/main.cpp
    src/PackageManager.cpp
    common/Logger.cpp
    common/ParamParser.cpp
)


add_executable(${EXECUTABLE_NAME} ${SOURCES})


target_link_libraries(${EXECUTABLE_NAME} PRIVATE CURL::libcurl nlohmann_json::nlohmann_json)


target_include_directories(${EXECUTABLE_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/src ${PROJECT_SOURCE_DIR}/common)


if (BUILD_CREATOR)
    add_subdirectory(creator)
endif()