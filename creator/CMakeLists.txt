set(EXECUTABLE_NAME bh-creator)


find_package(nlohmann_json 3.2.0 REQUIRED)

add_compile_definitions(ENABLE_COLOR_OUTPUT)


set(SOURCES
    src/main.cpp
    ../common/Logger.cpp
    ../common/ParamParser.cpp
)


add_executable(${EXECUTABLE_NAME} ${SOURCES})

target_link_libraries(${EXECUTABLE_NAME} PRIVATE CURL::libcurl nlohmann_json::nlohmann_json)


target_include_directories(${EXECUTABLE_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/src ${PROJECT_SOURCE_DIR}/common)