cmake_minimum_required(VERSION 3.5.1)
SET(CMAKE_SYSTEM_NAME Linux)

PROJECT(AML_DEMO_FACE_DETECT)
SET(EXECUTABLE_OUTPUT_PATH ../build)

link_directories(
	${NNSDK_LIB_DIR}
	${3rdparty_LIB_DIR}
	)

include_directories(
	${NNSDK_INCLUDE_DIR}
	${3rdparty_INCLUDE_DIR}
	${3rdparty_INCLUDE_DIR}/jpeg
	.
	)

aux_source_directory(
	. 
	SRC_LIST
	)

link_libraries(
	nnsdk 
	libjpeg.a 
	)

if(ARM64)
	ADD_EXECUTABLE(aml_face_rfb_64 ${SRC_LIST})
	target_link_libraries(aml_face_rfb_64 -lpthread -lm -ldl)
elseif(ARM32)
	ADD_EXECUTABLE(aml_face_rfb_32 ${SRC_LIST})
	target_link_libraries(aml_face_rfb_32 -lpthread -lm)
endif()