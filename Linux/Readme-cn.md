# 1、目录结构介绍

```
├── library                                          --- amlogic及第三方库文件、头文件
│   ├── 3rdparty
│   │   ├── include
│   │   └── lib
│   └── amlnnsdk
│       ├── include
│       ├── lib
│       └── release.md
├── out                                              --- 编译出的各平台demo可执行文件
│   ├── A311X2
│   │   ├── face_landmark5
│   │   ├── image_classify
│   │   └── object_detect_yolov5s
│   ├── C305X
│   │   ├── face_landmark5
│   │   ├── image_classify
│   │   └── object_detect_yolov5s
│   ├── C308L
│   │   ├── face_landmark5
│   │   ├── image_classify
│   │   └── object_detect_yolov5s
│   └── C308X
│       ├── face_landmark5
│       ├── image_classify
│       └── object_detect_yolov5s
├── Readme-cn.md
├── Readme.md
├── scripts                                          --- 编译脚本
│   ├── arm32_toolchain.cmake
│   ├── arm64_toolchain.cmake
│   ├── CMakeLists.txt
│   ├── make_nnsdk_32.sh
│   ├── make_nnsdk_64.sh
│   └── make_total.sh
└── source                                           --- 各平台的demo源文件，模型及输入
    ├── A311X2
    │   ├── face_landmark5
    │   ├── image_classify
    │   └── object_detect_yolov5s
    ├── C305X
    │   ├── face_landmark5
    │   ├── image_classify
    │   └── object_detect_yolov5s
    ├── C308L
    │   ├── face_landmark5
    │   ├── image_classify
    │   └── object_detect_yolov5s
    └── C308X
        ├── face_landmark5
        ├── image_classify
        └── object_detect_yolov5s
```

# 2、demo介绍

## 2.1、demo种类

- 当前提供了三种demo，分别是分类demo，目标检测demo以及人脸demo
- 分类demo
  - 分类 demo 通常用于对图像进行分类，即将输入的图像分到预定义的类别中的一种或多种，这些类别可以是猫、狗、汽车、飞机等等，demo的输出是每个类别的得分，即该图像或者视频帧属于每个类别的可能性，而 top5 得分则是指输出得分中最高的五个值，通常用于显示图像中最有可能的几个类别
- 检测demo
  - 检测 demo 用于检测图像中的目标对象，定位目标的位置，并输出目标的边界框坐标、类别和置信度。边界框坐标指的是目标对象在图像中的位置信息，类别是指目标对象所属的类别，置信度表示检测器对于该目标的可信程度
- 人脸demo
  - 人脸 demo 用于检测图像中的人脸，并输出人脸的位置信息、五个关键点的坐标和置信度，位置信息包括人脸的边界框坐标，五个关键点的坐标通常是指眼睛、鼻子和嘴巴等关键部位的位置，而置信度表示检测器对于该人脸的可信程度

## 2.2、调用接口

- 当前demo所调用的都是nnsdk的原生接口，各接口详细的用法请参考Amlogic Release包中的
  - DDK_x.x.x.x_SDK API.pdf
  - ADLA_SDK API (x.x.x).pdf
- 当前demo提供了三种输入模式，分别是
  - RGB24_RAW_DATA
  - BINARY_RAW_DATA
  - INPUT_DMA_DATA
  - 可以通过设置命令行参数argv[3]=0/1/2来选择使用的输入模式
- 当前demo提供了三种输出模式，分别是
  - AML_OUTDATA_FLOAT32
  - AML_OUTDATA_RAW
  - AML_OUTDATA_DMA
  - 可以通过设置命令行参数argv[4]=0/1/2来选择使用的输出模式

# 3、配置cmake编译环境

- 下载cmake
  - https://cmake.org/download/
  - 注：版本要高于3.5.1

- 安装cmake
  - tar -xvf cmake-3.*.*.tar
  - cd cmake-3.*.*
  - ./bootstrap 
  - make
  - make install
- 验证是否安装成功
  - cmake --version
  - 显示cmake 版本即为成功

# 4、配置编译工具链
-	下载工具链
	-	64位：wget https://releases.linaro.org/components/toolchain/binaries/6.3-2017.05/aarch64-linux-gnu/gcc-linaro-6.3.1-2017.05-x86_64_aarch64-linux-gnu.tar.xz
	-	32位：wget https://releases.linaro.org/components/toolchain/binaries/6.3-2017.05/arm-linux-gnueabihf/gcc-linaro-6.3.1-2017.05-x86_64_arm-linux-gnueabihf.tar.xz
-	aarch64-linux-gnu安装过程
	-	解压，并放置在自己需要的文件夹内 
		-	tar -xvJf ***.tar.xz
	-	编辑bash.bashrc文件 
		-	sudo vi ~/.bashrc
	-	添加变量 
		-	export PATH=path_to/gcc-linaro-6.3.1-2017.05-x86_64_aarch64-linux-gnu/bin:$PATH
	-	更新环境变量
		-	source ~/.bashrc
	-	检查环境是否加入成功
		-	echo $PATH
		-	下方出现刚添加的PATH即为成功
	-	运行aarch64-linux-gnu-gcc -v，查看gcc安装版本
	-	注：详细过程可参考：https://www.cnblogs.com/flyinggod/p/9468612.html

# 5、可执行文件编译

- 修改AML_NN_DEMO/Linux/scripts文件夹arm64_toolchain.cmake或者arm32_toolchain.cmake文件
  - 设置gcc-linaro路径：设置为第二步配置工具链中，解压后工具链放置目录。
  - 修改line 4-6 ，把路径改成自己本地的编译链路径:
    - SET(COMPILER /mnt/fileroot/xinxin.he/buildroot/toolchain/gcc/linux-x86/arm/gcc-linaro-6.3.1-2017.02-x86_64_arm-linux-gnueabihf/bin/)
    - SET(CMAKE_C_COMPILER ${COMPILER}/arm-linux-gnueabihf-gcc)
    - SET(CMAKE_CXX_COMPILER ${COMPILER}/arm-linux-gnueabihf-g++)

- 编译
  - 当前提供了一键编译所有demo的脚本
  - 执行命令：
    - cd AML_NN_DEMO/Linux/scripts
    - ./make_total.sh
    - 编译完成后会将各demo的可执行文件拷贝到AML_NN_DEMO/Linux/out对应的目录下

# 6、可执行文件运行

客户可以在平台上新建一个目录专门用于存放demo执行所需文件，如新建一个/data/case目录，之后将可执行文件、模型、输入push到平台上的/data/case目录下，以A311X2平台为例

- 分类demo

  - 在AML_NN_DEMO/Linux/下打开cmd，执行以下命令

    - push可执行文件：adb push out/A311X2/image_classify/aml_a311x2_image_classify_32 /data/case
    - push模型文件：adb push source/A311X2/image_classify/sdk_mobilenetv1_1e_A311X2.nb /data/case
    - push输入文件：adb push source/A311X2/image_classify/224x224x3_fish.jpeg /data/case

  - 板端切到/data/case目录下，执行如下命令

    - ./aml_a311x2_image_classify_32 sdk_mobilenetv1_1e_A311X2.nb 224x224x3_fish.jpeg 0 0

    ```
    top 0:score--15.631336,class--2
    top 1:score--11.977257,class--116
    top 2:score--10.048716,class--843
    top 3:score--9.744209,class--444
    top 4:score--9.236698,class--795
    ```

- 人脸demo

  - 在AML_NN_DEMO/Linux/下打开cmd，执行以下命令

    - push可执行文件：adb push out/A311X2/image_classify/aml_a311x2_face_landmark5_32 /data/case
    - push模型文件：adb push source/A311X2/image_classify/det_retinaface_1e_A311X2.nb /data/case
    - push输入文件：adb push source/A311X2/image_classify/640x384x3_tfboy.jpg /data/case

  - 板端切到/data/case目录下后，执行如下命令

    - ./aml_a311x2_face_landmark5_32 det_retinaface_1e_A311X2.nb 640x384x3_tfboy.jpg 0 0

    ```
    the number of detected faces is: 3
    face_number: 0, left: 258, top: 49, right: 365, bot: 190, prob: 0.994141
    face_number: 0, left_eye.x = 288.997, left_eye.y = 106.704
    face_number: 0, right_eye.x = 336.886, right_eye.y = 107.999
    face_number: 0, nose.x = 309.706, nose.y = 135.826
    face_number: 0, left_mouth.x = 293.527, left_mouth.y = 157.182
    face_number: 0, right_mouth.x = 332.356, right_mouth.y = 158.476
    face_number: 1, left: 531, top: 53, right: 636, bot: 186, prob: 0.989258
    face_number: 1, left_eye.x = 559.703, left_eye.y = 113.176
    face_number: 1, right_eye.x = 606.297, right_eye.y = 115.764
    face_number: 1, nose.x = 581.059, nose.y = 139.061
    face_number: 1, left_mouth.x = 563.586, left_mouth.y = 165.594
    face_number: 1, right_mouth.x = 601.12, right_mouth.y = 167.536
    face_number: 2, left: 5, top: 74, right: 104, bot: 197, prob: 0.989746
    face_number: 2, left_eye.x = 36.2328, left_eye.y = 129.823
    face_number: 2, right_eye.x = 80.2386, right_eye.y = 131.117
    face_number: 2, nose.x = 62.7657, nose.y = 155.709
    face_number: 2, left_mouth.x = 44.6457, left_mouth.y = 180.3
    face_number: 2, right_mouth.x = 77.65, right_mouth.y = 181.594
    ```

- 检测demo

  - 在AML_NN_DEMO/Linux/下打开cmd，执行以下命令

    - push可执行文件：adb push out/A311X2/image_classify/aml_a311x2_object_detect_yolov5s_32 /data/case
    - push模型文件：adb push source/A311X2/image_classify/yolov5s_1e_A311X2.nb /data/case
    - push输入文件：adb push source/A311X2/image_classify/640x384x3_tennis.jpg /data/case

  - 板端切到/data/case目录下后，执行如下命令

    - ./aml_a311x2_object_detect_yolov5s_32 yolov5s_1e_A311X2.nb 640x384x3_tennis.jpg 0 0

    ```
    detNum = 3
    obj_number: 0, class_id = 0, class_name = person, left = 135.995, top = 44.68, right = 312.134, bot = 304.798, prob = 0.912228
    obj_number: 1, class_id = 38, class_name = tennis racket, left = 66.7397, top = 90.5111, right = 179.661, bot = 143.089, prob = 0.862953
    obj_number: 2, class_id = 32, class_name = sports ball, left = 559.782, top = 147.473, right = 572.802, bot = 159.231, prob = 0.81998
    ```

# 7、FQA

- 如果想直接在板端运行示例demo，需要做到以下几点
  - C305X&C308X&A311X2平台driver版本是6.4.11
    - 查看方法：dmesg | grep Galcore
  - C308L平台driver版本是1.3.0
    - 查看方法：dmesg | grep adla
  - 保证nnsdk版本在2.6.4-2.6.6，当前amlnnsdk包中提供的为2.6.6版本的库文件
    - 查看方法：strings /usr/lib/libnnsdk.so | grep "NNSDK"