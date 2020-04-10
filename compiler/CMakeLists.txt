cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_COMPILER)
set(CMAKE_CXX_STANDARD 17)
project(wuvyc)


file(GLOB_RECURSE SRC_FILES "src/*.cpp" "lib/*.cpp")

add_executable(${PROJECT_NAME} ${SRC_FILES})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/include)