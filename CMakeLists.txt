project(serial2midi)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")


set(Src serial2midi.cpp include/Serial.hpp)

add_executable(serial2midi ${Src})
