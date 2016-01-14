cmake_minimum_required(VERSION 2.6)
project(quadcopter)

set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -std=c++11")


add_executable(quadcopter main.cpp)

install(TARGETS quadcopter RUNTIME DESTINATION bin)

add_subdirectory(pngwriter)
add_subdirectory(occupancygrid)
add_subdirectory(localization)
add_subdirectory(navigation)
add_subdirectory(sonar)

target_link_libraries(quadcopter navigate localizer occupancy_grid png_writer -lpng -lz)
