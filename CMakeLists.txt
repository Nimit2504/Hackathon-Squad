cmake_minimum_required(VERSION 3.10)
project(HackathonSquad CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Release flags — important for performance in the 5-minute window
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS_RELEASE "-O2 -Wall -Wextra -Wshadow")

add_executable(hackathon_squad main.cpp)

# Optional: copy the binary next to the Makefile for convenience
set_target_properties(hackathon_squad PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}
)
