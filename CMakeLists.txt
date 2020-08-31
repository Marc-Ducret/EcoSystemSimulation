cmake_minimum_required(VERSION 3.17)
project(EcoSystem)

set(CMAKE_CXX_STANDARD 14)

find_package(SFML 2.5 COMPONENTS graphics audio REQUIRED)

add_executable(EcoSystem src/main.cpp src/math_utils.h src/tile.h src/action.h src/map.h)

target_link_libraries(EcoSystem sfml-graphics sfml-audio)