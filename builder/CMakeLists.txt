# builder/CMakeLists.txt — mirror of creator/CMakeLists.txt
set(EXECUTABLE_NAME bh-builder)

set(SOURCES
    src/main.cpp
    src/CtxBuilder.cpp
    ../common/Logger.cpp
    ../common/ParamParser.cpp
)

add_executable(${EXECUTABLE_NAME} ${SOURCES})

target_link_libraries(${EXECUTABLE_NAME} PRIVATE
    CURL::libcurl
    nlohmann_json::nlohmann_json
)

target_include_directories(${EXECUTABLE_NAME} PRIVATE
    ${PROJECT_SOURCE_DIR}/common
    ${PROJECT_SOURCE_DIR}/src
)

# Driver script ships under /usr/share/blackhole/builder/
install(TARGETS ${EXECUTABLE_NAME} DESTINATION bin)
install(FILES driver.vs DESTINATION share/blackhole/builder OPTIONAL)

# Compile-time path so bh-builder can find driver.vs in the install tree.
# Override at configure time with -DBLACKHOLE_BUILDER_DRIVER_VS=...
set(BLACKHOLE_BUILDER_DRIVER_VS "${CMAKE_INSTALL_FULL_DATADIR}/blackhole/builder/driver.vs"
    CACHE STRING "Path to the installed driver.vs")
target_compile_definitions(${EXECUTABLE_NAME} PRIVATE
    BLACKHOLE_BUILDER_DRIVER_VS="${BLACKHOLE_BUILDER_DRIVER_VS}"
)
