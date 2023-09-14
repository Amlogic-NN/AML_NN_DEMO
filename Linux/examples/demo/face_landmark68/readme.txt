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
            
			只保留add_subdirectory(face_landmark68),即只编译face_landmark68 demo,默认不编译其余demo
			
二、可执行文件运行
	1、将demo/face_landmark68/package目录下的对应A311D平台的face_landmark_68_88_A311D.nb文件和60x60x1.jpeg输入文件，以及demo/face_landmark68/aml_face_land68_64可执行文件拷贝平台上，例如拷贝到/data路径下
	2、执行命令：
		./aml_face_land68_64 face_landmark_68_88_A311D.nb 60x60x1.jpeg
	   Note:  第二个参数 xxx.nb根据板子型号来选取。
	3、运行结果：
		the input type should be 60*60*1
		pos[0][0] x = 3.954746 , y = 22.033583
		pos[0][1] x = 5.649637 , y = 27.683220
		pos[0][2] x = 6.214601 , y = 34.180302
		pos[0][3] x = 9.039419 , y = 40.959866
		pos[0][4] x = 10.451828 , y = 48.869358
		pos[0][5] x = 14.124092 , y = 55.931404
		pos[0][6] x = 18.078838 , y = 58.191261
		pos[0][7] x = 23.445993 , y = 63.840897
		pos[0][8] x = 31.920448 , y = 64.688347
		pos[0][9] x = 39.264977 , y = 64.123375
		pos[0][10] x = 44.632130 , y = 60.451111
		pos[0][11] x = 49.434322 , y = 56.213886
		pos[0][12] x = 53.389069 , y = 48.021912
		pos[0][13] x = 56.496368 , y = 41.524830
		pos[0][14] x = 57.626297 , y = 33.615337
		pos[0][15] x = 57.908779 , y = 22.881029
		pos[0][16] x = 57.626297 , y = 14.971538
		pos[0][17] x = 11.581756 , y = 15.536501
		pos[0][18] x = 13.559128 , y = 14.406574
		pos[0][19] x = 14.124092 , y = 13.276647
		pos[0][20] x = 17.796356 , y = 14.689056
		pos[0][21] x = 22.598549 , y = 14.406574
		pos[0][22] x = 31.920448 , y = 12.711683
		pos[0][23] x = 36.722637 , y = 10.451828
		pos[0][24] x = 39.264977 , y = 10.169346
		pos[0][25] x = 43.784687 , y = 9.321901
		pos[0][26] x = 47.456951 , y = 10.734310
		pos[0][27] x = 28.248184 , y = 18.926283
		pos[0][28] x = 27.683220 , y = 24.010956
		pos[0][29] x = 27.965702 , y = 28.813148
		pos[0][30] x = 28.813148 , y = 35.875195
		pos[0][31] x = 24.010956 , y = 37.852566
		pos[0][32] x = 25.988329 , y = 39.547459
		pos[0][33] x = 28.530666 , y = 38.982494
		pos[0][34] x = 30.508039 , y = 38.700012
		pos[0][35] x = 34.180302 , y = 36.440159
		pos[0][36] x = 15.536501 , y = 19.208765
		pos[0][37] x = 17.796356 , y = 18.643803
		pos[0][38] x = 20.056211 , y = 17.513874
		pos[0][39] x = 22.881029 , y = 19.491247
		pos[0][40] x = 19.773729 , y = 18.361319
		pos[0][41] x = 18.078838 , y = 19.208765
		pos[0][42] x = 36.722637 , y = 15.536501
		pos[0][43] x = 37.005123 , y = 15.536501
		pos[0][44] x = 40.112423 , y = 14.971538
		pos[0][45] x = 42.372276 , y = 15.536501
		pos[0][46] x = 40.394905 , y = 16.948912
		pos[0][47] x = 36.440159 , y = 17.231392
		pos[0][48] x = 24.858402 , y = 48.304398
		pos[0][49] x = 25.423367 , y = 47.739433
		pos[0][50] x = 27.965702 , y = 48.021912
		pos[0][51] x = 29.378113 , y = 46.609505
		pos[0][52] x = 29.378113 , y = 46.891987
		pos[0][53] x = 33.332859 , y = 46.044540
		pos[0][54] x = 35.875195 , y = 45.762058
		pos[0][55] x = 33.615337 , y = 46.891987
		pos[0][56] x = 31.073002 , y = 46.609505
		pos[0][57] x = 28.813148 , y = 47.456951
		pos[0][58] x = 28.530666 , y = 47.739433
		pos[0][59] x = 25.423367 , y = 46.609505
		pos[0][60] x = 23.445993 , y = 48.869358
		pos[0][61] x = 27.683220 , y = 48.021912
		pos[0][62] x = 29.660593 , y = 49.434322
		pos[0][63] x = 31.355484 , y = 47.174465
		pos[0][64] x = 33.332859 , y = 46.044540
		pos[0][65] x = 31.637968 , y = 44.914612
		pos[0][66] x = 29.660593 , y = 45.762058
		pos[0][67] x = 27.965702 , y = 46.044540