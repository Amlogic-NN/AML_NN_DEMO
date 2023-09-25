一、可执行文件编译
	1、修改cmake文件夹arm64_toolchain.cmake或者arm32_toolchain.cmake文件
        设置gcc-linaro路径：设置为第二步配置工具链中，解压后工具链放置目录。
        修改line 4-6 :
        SET(COMPILER /mnt/fileroot/siqi.yang/C1/toolchain/gcc/linux-x86/aarch64/gcc-linaro-6.3.1-2017.02-x86_64_aarch64-linux-gnu/bin/)
        SET(CMAKE_C_COMPILER ${COMPILER}/aarch64-linux-gnu-gcc)
        SET(CMAKE_CXX_COMPILER ${COMPILER}/aarch64-linux-gnu-g++)

	2. 编译
        A. 以arm64为例，如果需要编译得到所有demo的可执行文件
        执行命令：
            ./make_nnsdk_linux_64.sh
            编译得到的可执行文件统一放在build/package文件夹下
        
        B. 修改demo文件夹下的CMakeLists.txt文件
            删除line 18-30:
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
            
			只保留add_subdirectory(head_detection),即只编译head_detection demo,默认不编译其余demo
			
二、可执行文件运行
	1、将demo/head_detection/package目录下的对应A311D平台的head_detect_88_A311D.nb文件和416x416x3.jpeg输入文件，以及demo/head_detection/aml_head_det_64可执行文件拷贝平台上，例如拷贝到/data路径下
	2、执行命令：
		./aml_head_det_64 head_detect_88_A311D.nb 416x416x3.jpeg
	   Note:  第二个参数 xxx.nb根据板子型号来选取。
	3、运行结果：
		the input type should be 416*416*3
		detect number =9 
		all_boxes:9; x1:0.069905; y1:0.013513; x2:0.311867; y2:0.211578
		all_boxes:9; x1:0.368259; y1:0.010173; x2:0.592973; y2:0.223439
		all_boxes:9; x1:0.742060; y1:-0.009538; x2:0.980557; y2:0.223517
		all_boxes:9; x1:0.051952; y1:0.251867; x2:0.276666; y2:0.465134
		all_boxes:9; x1:0.391366; y1:0.256860; x2:0.633328; y2:0.454924
		all_boxes:9; x1:0.665684; y1:0.241973; x2:0.922487; y2:0.475028
		all_boxes:9; x1:0.714719; y1:0.496878; x2:0.939433; y2:0.710144
		all_boxes:9; x1:0.059310; y1:0.517948; x2:0.268006; y2:0.731214
		all_boxes:9; x1:0.061647; y1:0.755596; x2:0.286361; y2:0.968862