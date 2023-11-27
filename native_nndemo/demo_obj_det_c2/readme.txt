一、目录结构
    example：示例源码
    nnsdk_vx.x.x：nnsdk_vx.x.x 动态库及头文件
    model_convert：模型转换示例脚本
    case：示例可执行文件及模型文件

二、模型转换流程
    model_convert目录中包含了原始模型、转换脚本、量化图片
    转换流程：
        1、拿到release的工具包，acuity-toolkit-binary-x.x.x.x
        2、将model_convert整个拷贝至工具包中跟demo同级路径下，然后依次执行脚本
        3、./cmd_import.sh	./cmd_quantize.sh           ./cmd_export.sh 0xbe
        4、转换后的nb模型文件存放在det_yolov2_0xbe路径下
    注：
        关于输入输出及量化参数的配置可参考《Model Transcoding and Running User Guide (1.1) CN.pdf》

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
		 ./case_obj_det_linux_64 det_yolov2_0xbe.nb space_shuttle_416x416_u8_nchw.bin
	3、运行结果：
                             obj_number:0,class:airplane,label num:4,prob:0.600094
                             left:-18.808651,right:387.168945,top:21.773304,bot:282.587006