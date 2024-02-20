cmake_minimum_required(VERSION 3.5.1)
SET(CMAKE_SYSTEM_NAME Linux)

PROJECT(DEMO_AML_C308X_IMAGE_CLASSIFY)

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


if(ARM32)
	ADD_EXECUTABLE(aml_c308x_image_classify_32 ${SRC_LIST})
	target_link_libraries(aml_c308x_image_classify_32 -lpthread -lm)
elseif(ARM64)
	ADD_EXECUTABLE(aml_c308x_image_classify_64 ${SRC_LIST})
	target_link_libraries(aml_c308x_image_classify_64 -lpthread -lm)
endif()