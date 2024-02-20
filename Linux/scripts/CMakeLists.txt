cmake_minimum_required(VERSION 3.5.1)
#SET(CMAKE_SYSTEM_NAME Linux)
PROJECT(AML_NNSDK)

add_definitions("-DBUILD_TIME=${BUILD_TIME}")
add_definitions(-DBUILD_LINUX)

SET(EXECUTABLE_OUTPUT_PATH ./build)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

find_package(Threads REQUIRED)
add_compile_options(-Wall)
add_compile_options(-Wno-unused-function)
add_compile_options(-Wno-deprecated-declarations)
add_compile_options(-Wno-unused-variable)
add_compile_options(-Wno-unused-but-set-variable)
add_compile_options(-Wno-sign-compare)

add_subdirectory(../source/A311X2/image_classify ./source/A311X2/image_classify)
add_subdirectory(../source/A311X2/face_landmark5 ./source/A311X2/face_landmark5)
add_subdirectory(../source/A311X2/object_detect_yolov5s ./source/A311X2/object_detect_yolov5s)

add_subdirectory(../source/C305X/image_classify ./source/C305X/image_classify)
add_subdirectory(../source/C305X/face_landmark5 ./source/C305X/face_landmark5)
add_subdirectory(../source/C305X/object_detect_yolov5s ./source/C305X/object_detect_yolov5s)

add_subdirectory(../source/C308X/image_classify ./source/C308X/image_classify)
add_subdirectory(../source/C308X/face_landmark5 ./source/C308X/face_landmark5)
add_subdirectory(../source/C308X/object_detect_yolov5s ./source/C308X/object_detect_yolov5s)

add_subdirectory(../source/C308L/image_classify ./source/C308L/image_classify)
add_subdirectory(../source/C308L/face_landmark5 ./source/C308L/face_landmark5)
add_subdirectory(../source/C308L/object_detect_yolov5s ./source/C308L/object_detect_yolov5s)