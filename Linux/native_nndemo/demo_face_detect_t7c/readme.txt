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

三、配置ndk编译环境：
	1、下载android-ndk-r21e
		https://github.com/android/ndk/wiki/Unsupported-Downloads
	2、 解压
	3、 设置环境变量
        export NDKROOT=/home/siqiyang/work/ndk/android-ndk-r21e
        export PATH=$NDKROOT:$PATH    

四、ndk编译
	1、在example/jni路径下执行ndk-build，可执行文件将保存在example\obj\local\arm64-v8a路径下
                2、将可执行文件拷贝至case目录下

五、可执行文件运行
	1、将当前整个case目录拷贝到/data目录下
	2、执行命令（argv[1]是可执行文件名称，argv[2]是adla模型名称，argv[3]是输入名称，以下以a311d2平台android64位环境为例）：
		./case_face_det_android_64 det_retinaface_int8.adla 640x384x3_i8.bin
		face_detNum = 3
		face_number:0, x:259.500000, y:48.500000, w:104.000000, h:142.000000, prob:0.992188
		face_number:1, x:532.500000, y:50.500000, w:102.000000, h:138.000000, prob:0.988281
		face_number:2, x:3.500000, y:71.000000, w:102.000000, h:129.000000, prob:0.988281
