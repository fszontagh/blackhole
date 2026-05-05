# tests/CMakeLists.txt
include(CTest)

set(TEST_SOURCES
    test_main.cpp
    test_packagemeta_v1.cpp
)

add_executable(blackhole_tests ${TEST_SOURCES})

target_include_directories(blackhole_tests PRIVATE
    ${PROJECT_SOURCE_DIR}/common
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/tests/third_party
)

target_link_libraries(blackhole_tests PRIVATE nlohmann_json::nlohmann_json)

add_test(NAME blackhole_tests COMMAND blackhole_tests)
