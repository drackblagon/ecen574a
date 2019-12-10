cmake_minimum_required(VERSION 3.4.1)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY "src/")
project(hlsyn)

#Bring the headers, such as Student.h into the project
include_directories(src)

#However, the file(GLOB...) allows for wildcard additions:
file(GLOB SOURCES "src/*.cpp" "src/*.h")

add_executable(hlsyn ${SOURCES})

# "../../../assignment3_testfiles_full/standard tests/hls_test1.c"
# "../../../results/hls_test1.v"
