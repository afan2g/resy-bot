cmake_minimum_required(VERSION 3.16)
project(resy_bot)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include FetchContent module
include(FetchContent)

# Find CURL
find_package(CURL REQUIRED)

# Download and configure nlohmann_json
FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.2
)


# Make both libraries available
FetchContent_MakeAvailable(json)

# Include directories
include_directories(include)

# Main executable
add_executable(resy_bot
    src/main.cpp
    src/config.cpp
    src/http.cpp
    src/utils.cpp
    src/captcha.cpp
)

# Link libraries
target_link_libraries(resy_bot
    PRIVATE
    CURL::libcurl
    nlohmann_json::nlohmann_json
)

# Create directory structure during build
add_custom_command(TARGET resy_bot POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/data
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/data/headers
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/data/booking_configs
)

# Documentation message
message(STATUS "Build configuration complete. After building, run from the build directory.")