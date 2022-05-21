project(serial2midi)

cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")

find_package(ALSA REQUIRED)

set(Src serial2midi.cpp include/Serial.hpp include/Midi.hpp)

add_executable(serial2midi ${Src})
target_link_libraries(serial2midi ${ALSA_LIBRARIES})
