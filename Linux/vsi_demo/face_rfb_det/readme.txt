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
            
			只保留add_subdirectory(face_rfb_det),即只编译face_rfb_det demo,默认不编译其余demo
			
二、可执行文件运行
	1、将demo/face_rfb_det/package目录下的对应A311D平台的face_detect_rfb_88_A311D.nb文件和320x320x3.jpeg输入文件，以及demo/face_rfb_det/aml_face_rfb_64可执行文件拷贝平台上，例如拷贝到/data路径下
	2、执行命令：
		./aml_face_rfb_64 face_detect_rfb_88_A311D.nb 320x320x3.jpeg
	   Note:  第二个参数 xxx.nb根据板子型号来选取。
	3、运行结果：
		the input type should be 320*320*3
		detect number =12 
		face_number:0 : box.x = 28.760361 , box.y = 9.014557, box.w = 56.256390, box.h = 64.638054 
		face_number:1 : box.x = 242.733215 , box.y = 5.392776, box.w = 58.533585, box.h = 69.976921 
		face_number:2 : box.x = 235.442642 , box.y = 89.815369, box.w = 54.067787, box.h = 66.590645 
		face_number:3 : box.x = 241.741028 , box.y = 173.141388, box.w = 53.534073, box.h = 63.368240 
		face_number:4 : box.x = 129.013428 , box.y = 8.871223, box.w = 54.068089, box.h = 64.638420 
		face_number:5 : box.x = 24.471088 , box.y = 91.080917, box.w = 53.534370, box.h = 62.123718 
		face_number:6 : box.x = 135.728058 , box.y = 91.929214, box.w = 54.068089, box.h = 60.903301 
		face_number:7 : box.x = 25.017876 , box.y = 173.536392, box.w = 51.964622, box.h = 59.117474 
		face_number:8 : box.x = 138.018784 , box.y = 170.618759, box.w = 58.533913, box.h = 64.000359 
		face_number:9 : box.x = 27.893303 , box.y = 255.494766, box.w = 51.451668, box.h = 58.533913 
		face_number:10 : box.x = 125.495911 , box.y = 252.437866, box.w = 56.817558, box.h = 62.743073 
		face_number:11 : box.x = 241.450287 , box.y = 251.809204, box.w = 57.956116, box.h = 64.000359 
