cmake_minimum_required(VERSION 3.5.1)
#SET(CMAKE_SYSTEM_NAME Linux)
PROJECT(AML_NNSDK)

add_definitions("-DBUILD_TIME=${BUILD_TIME}")
add_definitions(-DBUILD_LINUX)

SET(EXECUTABLE_OUTPUT_PATH ./build)
set(CMAKE_C_FLAGS“$ {CMAKE_C_FLAGS} -fPIC”)
set(CMAKE_CXX_FLAGS“$ {CMAKE_CXX_FLAGS} -fPIC”)

find_package(Threads REQUIRED)
add_compile_options(-Wall)
add_compile_options(-Wno-unused-function)
add_compile_options(-Wno-deprecated-declarations)
add_compile_options(-Wno-unused-variable)
add_compile_options(-Wno-unused-but-set-variable)

add_subdirectory(demo)
