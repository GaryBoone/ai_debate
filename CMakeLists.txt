cmake_minimum_required(VERSION 3.5)

project(openai_client)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON) 

# Add more warnings than -Wall. 
# These enable warnings in VSCode as well.
add_definitions("-Wreorder -Wunused -Wparentheses -Wimplicit-fallthrough -Wreturn-type -Wuninitialized -Winit-self")

# We need this options for generating compile_commands.json file
# It's required for clang static analyzer and autocompletion tools based on clang
# PVS uses it too
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# In addition to the above, to enable the unused includes diagnostic in VSCode,
# create the following .clangd file in the project root directory:
# Diagnostics:
#   UnusedIncludes: Strict

# The main source file.
set(MAIN_SOURCE
  src/main.cpp 
)

# Source files without the main file.
set(APP_SOURCES
  src/gpt_client/gpt_client.cpp 
  src/gpt_client/chunk_processor.cpp
  src/claude_client/claude_client.cpp 
  src/claude_client/chunk_processor.cpp
  src/gemini_client/gemini_client.cpp 
  src/gemini_client/chunk_processor.cpp
  src/api_client/api_client.cpp
  src/api_client/api_stream_handler.cpp
  src/api_client/gpt_chunk_processor.cpp
  src/api_client/gpt_request_maker.cpp
  src/api_client/claude_chunk_processor.cpp
  src/api_client/claude_request_maker.cpp
  src/api_client/gemini_chunk_processor.cpp
  src/api_client/gemini_request_maker.cpp
  src/util/stacktrace.cpp
)

# Library files.
set(DEP_LIBS
  cpr::cpr 
  nlohmann_json::nlohmann_json
  cpptrace::cpptrace
)

# For each included library, the git command is given for checking the tags.
# Note that the git command returns them in lexicographical order. So, for
# example, v1.9.0 comes after v1.10.0 because as strings "v1.9" is sorted after
# "v1.1". However, these tags represent semantic versioning, which means that
# each part of the version tag is a separate number. So, v1.10.0 is laaer
# version than v1.9.0.
include(FetchContent)
# Include expected for error handling.
# To check the tags,
# $ git ls-remote --tags https://github.com/TartanLlama/expected.git
FetchContent_Declare(
  expected
  GIT_REPOSITORY https://github.com/TartanLlama/expected.git
  GIT_TAG        v1.1.0  # origin/master
)
FetchContent_MakeAvailable(expected)
message(STATUS "expected_SOURCE_DIR: ${expected_SOURCE_DIR}")

# Include cpptrace for better stack traces.
# To check the tags,
# $ git ls-remote --tags https://github.com/jeremy-rifkin/cpptrace.git
FetchContent_Declare(
  cpptrace
  GIT_REPOSITORY https://github.com/jeremy-rifkin/cpptrace.git
  GIT_TAG v0.5.2  # main
)
FetchContent_MakeAvailable(cpptrace)
message(STATUS "cpptrace_SOURCE_DIR: ${cpptrace_SOURCE_DIR}")

# Include cpr for HTTP requests.
# To check the tags,
# $ git ls-remote --tags https://github.com/libcpr/cpr.git
include(FetchContent)
FetchContent_Declare(
  cpr 
  GIT_REPOSITORY https://github.com/libcpr/cpr.git
  GIT_TAG 1.10.5
)
FetchContent_MakeAvailable(cpr)
message(STATUS "cpr_SOURCE_DIR: ${cpr_SOURCE_DIR}")

# Include nlohmann/json for JSON parsing.
# To check the tags,
# $ git ls-remote --tags https://github.com/nlohmann/json.git
FetchContent_Declare(
  json 
  URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
)
FetchContent_MakeAvailable(json)
message(STATUS "json_SOURCE_DIR: ${json_SOURCE_DIR}")

# Include doctest for testing.
# To check the tags,
# $ git ls-remote --tags https://github.com/onqtam/doctest
FetchContent_Declare(
  doctest 
  GIT_REPOSITORY "https://github.com/onqtam/doctest"
  GIT_TAG v2.4.9
) 
FetchContent_MakeAvailable(doctest)
message(STATUS "doctest_SOURCE_DIR: ${doctest_SOURCE_DIR}")
include_directories(${doctest_SOURCE_DIR}) 

# Avoid `file(GLOB SOURCES src/*.cpp)` because it won't pick up new files
# automatically.
add_executable(openai_app ${MAIN_SOURCE} ${APP_SOURCES})
target_link_libraries(openai_app PRIVATE ${DEP_LIBS})


# Define the tests.
set(TEST_SOURCES
  tests/chunk_processor_test.cpp
)

add_executable(openai_tests ${APP_SOURCES} ${TEST_SOURCES})
target_link_libraries(openai_tests PRIVATE ${DEP_LIBS} doctest)


# Handle OPENAI_API_KEY with an option and proper usage instructions 
option(USE_ENV_API_KEY "Use OPENAI_API_KEY environment variable if set" ON)

if(USE_ENV_API_KEY)
  add_definitions(-DUSE_ENV_API_KEY)
else()
  message(STATUS "Provide OPENAI_API_KEY via -DOPENAI_API_KEY=your_key during CMake configuration")
endif()

