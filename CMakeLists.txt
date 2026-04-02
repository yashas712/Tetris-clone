cmake_minimum_required(VERSION 3.16)
project(Games)

set(CMAKE_CXX_STANDARD 20)

# Tell CMake where to find SFML
set(SFML_DIR "/usr/local/cmake/lib/cmake/SFML")

find_package(SFML 3 REQUIRED COMPONENTS Graphics Window System)

add_executable(Games
        tetris.cpp)

target_link_libraries(Games SFML::Graphics SFML::Window SFML::System)