cmake_minimum_required(VERSION 3.10)

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

# Include cpptrace for better stack traces.
include(FetchContent)
FetchContent_Declare(
  cpptrace
  GIT_REPOSITORY https://github.com/jeremy-rifkin/cpptrace.git
  GIT_TAG main
)
FetchContent_MakeAvailable(cpptrace)
# Force the correct casing for the include path
get_filename_component(CPPTRACE_INCLUDE_DIR ${cpptrace_SOURCE_DIR}/Include ABSOLUTE) 
include_directories(${CPPTRACE_INCLUDE_DIR}) 

# Define the main executable and link the required libraries.
find_package(cpr REQUIRED)
find_package(nlohmann_json REQUIRED) 
include_directories(${cpptrace_SOURCE_DIR})  # After FetchContent_MakeAvailable
message(STATUS "cpptrace_SOURCE_DIR: ${cpptrace_SOURCE_DIR}")
message(STATUS "CPPTRACE_INCLUDE_DIR: ${CPPTRACE_INCLUDE_DIR}")

# Avoid `file(GLOB SOURCES src/*.cpp)` because it won't pick up new files automatically.
add_executable(openai_app ${MAIN_SOURCE} ${APP_SOURCES})
target_link_libraries(openai_app PRIVATE ${DEP_LIBS})


# Define the tests.
set(TEST_SOURCES
  tests/chunk_processor_test.cpp
)
find_package(Catch2 REQUIRED)
add_executable(openai_tests ${APP_SOURCES} ${TEST_SOURCES})
target_link_libraries(openai_tests PRIVATE Catch2::Catch2WithMain ${DEP_LIBS}) 


# Handle OPENAI_API_KEY with an option and proper usage instructions 
option(USE_ENV_API_KEY "Use OPENAI_API_KEY environment variable if set" ON)

if(USE_ENV_API_KEY)
  add_definitions(-DUSE_ENV_API_KEY)
else()
  message(STATUS "Provide OPENAI_API_KEY via -DOPENAI_API_KEY=your_key during CMake configuration")
endif()

