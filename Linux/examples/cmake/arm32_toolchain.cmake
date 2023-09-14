set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

SET(COMPILER /mnt/fileroot/siqi.yang/C1/toolchain/gcc/linux-x86/arm/gcc-linaro-6.3.1-2017.02-x86_64_arm-linux-gnueabihf/bin)
SET(CMAKE_C_COMPILER ${COMPILER}/arm-linux-gnueabihf-gcc)
SET(CMAKE_CXX_COMPILER ${COMPILER}/arm-linux-gnueabihf-g++)

SET(NNSDK_LIB_DIR ../../sharelib/lib32 )
SET(3rdparty_LIB_DIR ../../3rdparty/lib32 )
SET(NNSDK_INCLUDE_DIR ../../include )
SET(3rdparty_INCLUDE_DIR ../../3rdparty/include )

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)






