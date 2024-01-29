一、目录结构
    example：示例源码
    nnsdk_vx.x.x：nnsdk_vx.x.x 动态库及头文件
    model_convert：模型转换示例脚本
    case：示例可执行文件及模型文件

二、模型转换流程
    model_convert目录中包含了原始模型、转换脚本
    转换流程：
        1，拿到release的工具包，adla-toolkit-binary-x.x.x.x
        2.  将model_convert中的convert_adla.sh替换adla-toolkit-binary-x.x.x.x/demo/中的convert_adla.sh
        3.  将model_convert中的det_retinaface.pb拷贝至adla-toolkit-binary-x.x.x.x/demo/model_source/tensorflow_model
        4. ./convert_adla.sh
        5、转换后的adla模型文件存放在adla-toolkit-binary-x.x.x.x/demo/tensorflow_output路径下
    注：
        关于模型转换的流程、输入输出及量化参数的配置可参考《Model Conversion User Guide (x.x)_CN.pdf》

三、配置cmake编译环境：
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

四、配置编译工具链

五、可执行文件编译
	1、修改CMakeLists.txt
		A、修改GCC_PATH：
			修改line 5 :SET(GCC_PATH /xxx/xxx)
			设置gcc-linaro路径：设置为第二步配置工具链中，解压后工具链放置目录。
			例如，我电脑上放置的路径是:/mnt/fileroot/xinxin.he/buildroot/toolchain/gcc/linux-x86
			即设置：set(GCC_PATH /mnt/fileroot/xinxin.he/buildroot/toolchain/gcc/linux-x86)

		B、默认编译arm32可执行文件：
		注：用户自建工程可将CMakeLists.txt文件拷贝到工程main函数同级目录下。

	2、编译
		在nnsdk路径下执行命令：
		A、cmake .    //当前路径生成makefile
		B、make      //编译
		生成的可执行文件在当前目录下
		C、将可执行文件拷贝至case目录下

六、可执行文件运行
	1、将当前整个case目录拷贝到/目录下
	2、执行命令：
		 ./case_face_recog_linux_32 det_insightface_int8.adla 112x112x3_i8.bin
	3、运行结果：
		打印输出buf，是一个512维的向量