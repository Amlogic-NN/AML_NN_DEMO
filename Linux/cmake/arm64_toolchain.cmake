set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)

SET(COMPILER /mnt/fileroot/siqi.yang/C1/toolchain/gcc/linux-x86/aarch64/gcc-linaro-6.3.1-2017.02-x86_64_aarch64-linux-gnu/bin/)
SET(CMAKE_C_COMPILER ${COMPILER}/aarch64-linux-gnu-gcc)
SET(CMAKE_CXX_COMPILER ${COMPILER}/aarch64-linux-gnu-g++)

SET(NNSDK_LIB_DIR ../../sharelib/lib64 )
SET(3rdparty_LIB_DIR ../../3rdparty/lib64 )
SET(NNSDK_INCLUDE_DIR ../../include )
SET(3rdparty_INCLUDE_DIR ../../3rdparty/include )

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

