cmake_minimum_required(VERSION 3.5.1)
PROJECT(AML_NNSDK_DEMO)

add_definitions("-DBUILD_TIME=${BUILD_TIME}")
add_definitions(-DBUILD_LINUX)

include_directories(${VIVANTE_SDK_INC_DIRS})
include_directories(${OVXLIB_DIRS})

SET(EXECUTABLE_OUTPUT_PATH ../build)

find_package(Threads REQUIRED)
add_compile_options(-Wall)
add_compile_options(-Wno-unused-function)
add_compile_options(-Wno-deprecated-declarations)
add_compile_options(-Wno-unused-variable)

add_subdirectory(face_age)
add_subdirectory(face_compare)
add_subdirectory(face_detection)
add_subdirectory(face_emotion)
add_subdirectory(face_gender)
add_subdirectory(face_landmark5)
add_subdirectory(face_landmark68)
add_subdirectory(face_recognize)
add_subdirectory(face_rfb_det)
add_subdirectory(head_detection)
add_subdirectory(image_classify)
add_subdirectory(image_segmentation)
add_subdirectory(object_detect)

