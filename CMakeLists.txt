cmake_minimum_required(VERSION 2.6)
project(quadcopter)

set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -std=c++11")


add_executable(quadcopter main.cpp)

install(TARGETS quadcopter RUNTIME DESTINATION bin)

add_subdirectory(pngwriter)
add_subdirectory(resizablegrid)
add_subdirectory(localization)
target_link_libraries(quadcopter localizer resizable_grid png_writer -lpng -lz)