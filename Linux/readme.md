# 1、配置cmake编译环境

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

# 2、配置编译工具链
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

# 3、目录结构介绍

```
├── 3rdparty                               --- 第三方库及头文件
│   ├── include
│   │   └── jpeg
│   └── lib
│       ├── lib32
│       └── lib64
├── amlnnsdk                               --- amlnnsdk库及头文件
│   ├── include
│   │   ├── nn_sdk.h
│   │   ├── postprocess.h
│   │   └── utils.h
│   ├── lib
│   │   ├── lib32
│   │   └── lib64
│   └── release.md                         --- nnsdk release note
├── cmake                                  --- cmake选项配置文件
│   ├── arm32_toolchain.cmake
│   └── arm64_toolchain.cmake
├── CMakeLists.txt
├── demo                                   --- demo源文件, 编译脚本
│   ├── adla
│   │   ├── aml_face_landmark5
│   │   ├── aml_image_classify
│   │   ├── aml_object_detect_yolov5s
│   │   └── CMakeLists.txt
│   └── vsi
│       ├── aml_face_landmark5
│       ├── aml_image_classify
│       ├── aml_object_detect_yolov5s
│       └── CMakeLists.txt
├── make_nnsdk_adla_32.sh                  --- nnsdk adla demo 64位编译脚本
├── make_nnsdk_vsi_32.sh                   --- nnsdk vsi demo 32位编译脚本
├── make_nnsdk_vsi_64.sh                   --- nnsdk vsi demo 64位编译脚本
├── make_total.sh                          --- 总编译脚本
├── readme.md
└── test_case                              --- demo可执行文件, 运行相关的模型及输入文件
    ├── adla                               --- adla平台demo：C308L
    │   ├── aml_face_landmark5
    │   ├── aml_image_classify
    │   ├── aml_object_detect_yolov5s
    │   └── result.txt
    └── vsi                                --- vsi平台demo：C305X, C308X, A311X2
        ├── aml_face_landmark5
        ├── aml_image_classify
        ├── aml_object_detect_yolov5s
        └── result.txt
```

# 4、可执行文件编译
- 修改cmake文件夹arm64_toolchain.cmake或者arm32_toolchain.cmake文件
  - 设置gcc-linaro路径：设置为第二步配置工具链中，解压后工具链放置目录。
  - 修改line 4-6 :
    - SET(COMPILER /mnt/fileroot/siqi.yang/C1/toolchain/gcc/linux-x86/aarch64/gcc-linaro-6.3.1-2017.02-x86_64_aarch64-linux-gnu/bin/)
    - SET(CMAKE_C_COMPILER ${COMPILER}/aarch64-linux-gnu-gcc)
    - SET(CMAKE_CXX_COMPILER ${COMPILER}/aarch64-linux-gnu-g++)

- 编译
  - 当前提供了一键编译所有demo的脚本
  - 执行命令：
    - ./make_total.sh
    - 编译完成后会将各demo的可执行文件拷贝到test_case对应的目录下

# 5、可执行文件运行

- ADLA平台以C308L为例
  - 将test_case/adla目录push到平台上，例如拷贝到/data路径下
  - cd到各demo目录下，执行如下命令即可获取结果
    - Note:  第二个参数 xxx.adla根据板子型号来选取，当前adla平台只有C308L是linux环境

```
./aml_image_classify_adla_32 sdk_mobilenetv1_int8_C308L.adla 224x224x3_fish.jpeg 0 0
top 0:score--18.807236,class--2
top 1:score--12.043229,class--125
top 2:score--11.053375,class--120
top 3:score--11.053375,class--121
top 4:score--11.053375,class--141

./aml_face_landmark5_adla_32 det_retinaface_int8_C308L.adla 640x384x3_tfboy.jpg 0 0
the number of detected faces is: 3
face_number: 0, left: 260.5, top: 50.5, right: 362.5, bot: 188.5, prob: 0.996094
face_number: 0, left_eye.x = 289.477, left_eye.y = 107.521
face_number: 0, right_eye.x = 336.111, right_eye.y = 110.391
face_number: 0, nose.x = 307.413, nose.y = 136.936
face_number: 0, left_mouth.x = 293.064, left_mouth.y = 156.307
face_number: 0, right_mouth.x = 329.654, right_mouth.y = 158.459
face_number: 1, left: 517.5, top: 50.5, right: 617.5, bot: 188.5, prob: 0.992188
face_number: 1, left_eye.x = 561.978, left_eye.y = 112.543
face_number: 1, right_eye.x = 607.177, right_eye.y = 113.978
face_number: 1, nose.x = 581.349, nose.y = 139.806
face_number: 1, left_mouth.x = 565.565, left_mouth.y = 165.634
face_number: 1, right_mouth.x = 600.002, right_mouth.y = 166.351
face_number: 2, left: 0.00064, top: 70.5, right: 97.0006, bot: 200.5, prob: 0.992188
face_number: 2, left_eye.x = 35.4128, left_eye.y = 130.695
face_number: 2, right_eye.x = 77.0245, right_eye.y = 128.543
face_number: 2, nose.x = 61.2407, nose.y = 156.523
face_number: 2, left_mouth.x = 44.0221, left_mouth.y = 182.351
face_number: 2, right_mouth.x = 75.5896, right_mouth.y = 180.916

./aml_object_detect_yolov5s_adla_32 yolov5s_int8_C308L.adla 640x384x3_tennis.jpg 0 0
detNum = 3
obj_number: 0, class_id = 38, class_name = tennis racket, left = 67.3986, top = 92.9168, right = 173.305, bot = 136.768, prob = 0.814561
obj_number: 1, class_id = 0, class_name = person, left = 145.458, top = 47.0187, right = 317.839, bot = 296.482, prob = 0.804661
obj_number: 2, class_id = 32, class_name = sports ball, left = 558.941, top = 147.807, right = 572.882, bot = 159.273, prob = 0.835863
```

- VSI平台已A311X2为例
  - 将test_case/vsi目录push到平台上，例如拷贝到/data路径下
  - cd到各demo目录下，执行如下命令即可获取结果
    - Note:  第二个参数 xxx.nb根据板子型号来选取

```
./aml_image_classify_vsi_32 sdk_mobilenetv1_1e_A311X2.nb 224x224x3_fish.jpeg 0 0
top 0:score--15.631336,class--2
top 1:score--11.977257,class--116
top 2:score--10.048716,class--843
top 3:score--9.744209,class--444
top 4:score--9.236698,class--795

./aml_face_landmark5_vsi_32 det_retinaface_1e_A311X2.nb 640x384x3_tfboy.jpg 0 0
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

./aml_object_detect_yolov5s_vsi_32 yolov5s_1e_A311X2.nb 640x384x3_tennis.jpg 0 0
detNum = 3
obj_number: 0, class_id = 0, class_name = person, left = 135.995, top = 44.68, right = 312.134, bot = 304.798, prob = 0.912228
obj_number: 1, class_id = 38, class_name = tennis racket, left = 66.7397, top = 90.5111, right = 179.661, bot = 143.089, prob = 0.862953
obj_number: 2, class_id = 32, class_name = sports ball, left = 559.782, top = 147.473, right = 572.802, bot = 159.231, prob = 0.81998
```

# 6、FQA

- 如果想直接在板端运行示例demo，需要做到以下几点
  - C305X&C308X&A311X2平台driver版本是6.4.11
    - 查看方法：dmesg | grep Galcore
  - C308L平台driver版本是1.3.0
    - 查看方法：dmesg | grep adla
  - 保证nnsdk版本在2.6.4以上
    - 查看方法：strings /usr/lib/libnnsdk.so | grep "NNSDK"