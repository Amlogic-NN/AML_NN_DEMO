一、配置cmake编译环境：
	1、下载cmake
		https://cmake.org/download/
		注：版本要高于3.5.1
	2、 安装cmake
		tar -xvf cmake-3.*.*.tar
		cd cmake-3.*.*
		./bootstrap 
		make
		make install
	3、 验证是否安装成功
		cmake --version
		显示cmake 版本即为成功

二、配置编译工具链
	1、 下载工具链
		64位：wget https://releases.linaro.org/components/toolchain/binaries/6.3-2017.05/aarch64-linux-gnu/gcc-linaro-6.3.1-2017.05-x86_64_aarch64-linux-gnu.tar.xz
		32位：wget https://releases.linaro.org/components/toolchain/binaries/6.3-2017.05/arm-linux-gnueabihf/gcc-linaro-6.3.1-2017.05-x86_64_arm-linux-gnueabihf.tar.xz
	2、 aarch64-linux-gnu安装过程
		a、解压，并放置在自己需要的文件夹内 
			tar -xvJf ***.tar.xz
		b、编辑bash.bashrc文件 
			sudo vi ~/.bashrc
		c、添加变量   
			export PATH=path_to/gcc-linaro-6.3.1-2017.05-x86_64_aarch64-linux-gnu/bin:$PATH
		d、更新环境变量   
			source ~/.bashrc
		e、检查环境是否加入成功   
			echo $PATH     
			下方出现刚添加的PATH即为成功
		f、运行aarch64-linux-gnu-gcc -v，查看gcc安装版本
    注：详细过程可参考：https://www.cnblogs.com/flyinggod/p/9468612.html

三、目录结构介绍
    ├── 3rdparty                                            --- 第三方库及头文件
    │   ├── include                                         --- 包含解析jpeg所需的头文件及opencv相关头文件
    │   ├── lib32                                           --- 包含32位解析jpeg所需的库文件、opencv相关库文件及libz.so
    │   └── lib64                                           --- 包含64位解析jpeg所需的头文件、opencv相关头文件及libz.so
    ├── cmake                                               --- cmake选项配置文件
    │   ├── arm32_toolchain.cmake
    │   └── arm64_toolchain.cmake
    ├── CMakeLists.txt                                  
    ├── demo                                                --- demo源文件、编译脚本、运行相关的nbg及输入文件
    │   ├── bodypose
    │   ├── CMakeLists.txt
    │   ├── face_age
    │   ├── face_compare
    │   ├── face_detection
    │   ├── face_emotion
    │   ├── face_gender
    │   ├── face_landmark5
    │   ├── face_landmark68
    │   ├── face_recognize
    │   ├── face_rfb_det
    │   ├── head_detection
    │   ├── image_classify
    │   ├── image_segmentation
    │   └── object_detect
    ├── include                                             --- nnsdk头文件
    │   ├── nn_sdk.h
    │   └── nn_util.h
    ├── make_nnsdk_linux_32.sh                              --- nnsdk demo 32位编译脚本
    ├── make_nnsdk_linux_64.sh                              --- nnsdk demo 64位编译脚本
    ├── readme.txt                                          --- readme文件
    └── sharelib                                            --- nnsdk库文件
        ├── lib32
        └── lib64

四、可执行文件编译
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
        
        B. 如果需要编译特定的demo，修改demo文件夹下的CMakeLists.txt文件
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
            当前默认为编译所有的demo，例如只需要编译face_detection demo,则可以将add_subdirectory(其他demo)的语句删除掉。

五、可执行文件运行
    以face_detection和64位A311D平台为例：
	1、将demo/face_detection/package目录下的对应A311D平台的face_detect_88_A311D.nb文件和640x384x3.jpeg输入文件，以及demo/face_detection/aml_face_det_64可执行文件拷贝平台上，例如拷贝到/data路径下
	2、执行命令：
		./aml_face_det_64 face_detect_88_A311D.nb 640x384x3.jpeg
	   Note:  第二个参数 xxx.nb根据板子型号来选取。
	3、运行结果：
		待补充