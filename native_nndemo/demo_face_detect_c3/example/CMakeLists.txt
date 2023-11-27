cmake_minimum_required(VERSION 3.5.1)
SET(CMAKE_SYSTEM_NAME Linux)
PROJECT(AML_NNSDK)

set(GCC_PATH /mnt/fileroot/xinxin.he/buildroot/toolchain/gcc/linux-x86)
set(COMPILER ${GCC_PATH}/arm/gcc-arm-10.3-2021.07-x86_64-arm-none-linux-gnueabihf/bin)
set(CMAKE_C_COMPILER ${COMPILER}/arm-none-linux-gnueabihf-gcc)
set(CMAKE_CXX_COMPILER ${COMPILER}/arm-none-linux-gnueabihf-g++)

include_directories(../nnsdk_v2.5.0/include ../nnsdk_v2.5.0/include/jpeg)
aux_source_directory(. SRC_LIST)
link_directories(../nnsdk_v2.5.0/lib/lib32)
link_libraries(nnsdk libjpeg.a)

ADD_EXECUTABLE(case_face_det_linux_32 ${SRC_LIST})
target_link_libraries(case_face_det_linux_32 -lpthread -lm -ldl)