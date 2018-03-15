cmake_minimum_required(VERSION 2.6)

project(CameraTracker C CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -ggdb -pthread")

file(GLOB srcFiles src/*.cpp)
file(GLOB includeFiles include/c_library_v2/common/*.cpp)

find_package(Boost 1.40 COMPONENTS program_options system REQUIRED)
include_directories(${BOOST_INCLUDE_DIR})

add_executable(CameraTracker ${srcFiles} ${includeFiles})

target_link_libraries(CameraTracker ${Boost_LIBRARIES})
