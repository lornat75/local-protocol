cmake_minimum_required(VERSION 2.6)

find_package(YARP REQUIRED)

include_directories(${YARP_INCLUDE_DIRS})

add_executable(main main.cpp)

target_link_libraries(main ${YARP_LIBRARIES})

