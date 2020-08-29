cmake_minimum_required(VERSION 3.17)
project(EcoSystem)

set(CMAKE_CXX_STANDARD 14)

find_package(SFML 2.5 COMPONENTS graphics audio REQUIRED)

add_executable(EcoSystem main.cpp)

target_link_libraries(EcoSystem sfml-graphics sfml-audio)