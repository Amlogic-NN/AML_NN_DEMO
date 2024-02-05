# 1、Introduction to Catalog Structure

```
├── 3rdparty                                          --- Third-party libraries and headers
│   ├── include
│   │   └── jpeg
│   └── lib
│       ├── lib32
│       └── lib64
├── amlnnsdk                                          --- amlnnsdk library, headers, release note
│   ├── include
│   │   ├── nn_sdk.h
│   │   ├── postprocess.h
│   │   └── utils.h
│   ├── lib
│   │   ├── lib32
│   │   └── lib64
│   └── release.md
├── cmake                                             --- cmake options configuration file
│   ├── arm32_toolchain.cmake
│   └── arm64_toolchain.cmake
├── CMakeLists.txt
├── demo                                              --- demo source files, compiled scripts
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
├── make_nnsdk_32.sh                                  --- nnsdk demo 32-bit compilation scripts
├── make_nnsdk_64.sh                                  --- nnsdk demo 64-bit compilation scripts
├── make_total.sh                                     --- Total compilation script
├── readme.md
└── test_case                                         --- The demo executable, which runs the associated model and input files
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

# 2、Demo introduction

## 2.1、Demo Type

- 当前提供了三种demo，分别是分类demo，目标检测demo以及人脸demo
- 分类demo在执行后可以得到top5得分
- 检测demo在执行后可以得到所检测到的目标数量，框坐标，类别，置信度
- 人脸demo在执行后可以得到所检测到的人脸数量，框坐标，五点坐标，置信度

## 2.2、Interface for Demo

- The current demo calls nnsdk's native interfaces, please refer to the Amlogic Release package for detailed usage of each interface.
  - DDK_x.x.x.x_SDK API.pdf
  - ADLA_SDK API (x.x.x).pdf
- The current demo provides three input modes, which are
  - RGB24_RAW_DATA
  - BINARY_RAW_DATA
  - INPUT_DMA_DATA
  - The input mode used can be selected by setting the command line parameter argv[3]=0/1/2
- The current demo provides three output modes, which are
  - AML_OUTDATA_FLOAT32
  - AML_OUTDATA_RAW
  - AML_OUTDATA_DMA
  - The output mode used can be selected by setting the command line parameter argv[4]=0/1/2

# 3、Configure the cmake compilation environment

- Download cmake
  - https://cmake.org/download/
  - Note: Version should be higher than 3.5.1

- Install cmake
  - tar -xvf cmake-3.*.*.tar
  - cd cmake-3.*.*
  - ./bootstrap 
  - make
  - make install
- Verify that the installation was successful
  - cmake --version
  - Showing the cmake version is a success

# 4、Configuring the compilation toolchain
-	Download toolchain
	-	64-bit：wget https://releases.linaro.org/components/toolchain/binaries/6.3-2017.05/aarch64-linux-gnu/gcc-linaro-6.3.1-2017.05-x86_64_aarch64-linux-gnu.tar.xz
	-	32-bit：wget https://releases.linaro.org/components/toolchain/binaries/6.3-2017.05/arm-linux-gnueabihf/gcc-linaro-6.3.1-2017.05-x86_64_arm-linux-gnueabihf.tar.xz
-	aarch64-linux-gnu installation process
	-	Unzip it and place it in your desired folder
		-	tar -xvJf ***.tar.xz
	-	Editing the bash.bashrc file
		-	sudo vi ~/.bashrc
	-	Adding Variables
		-	export PATH=path_to/gcc-linaro-6.3.1-2017.05-x86_64_aarch64-linux-gnu/bin:$PATH
	-	Updating environment variables
		-	source ~/.bashrc
	-	Check if the environment was joined successfully
		-	echo $PATH
		-	If the PATH you just added appears at the bottom, it's a success!
	-	Run aarch64-linux-gnu-gcc -v to see what version of gcc is installed
	-	Note: The detailed process can be referred to:https://www.cnblogs.com/flyinggod/p/9468612.html

# 5、Executable file compilation

- Modify the cmake folder arm64_toolchain.cmake or arm32_toolchain.cmake file
  - Set the path to gcc-linaro: set it to the directory where the toolchain will be placed after unpacking in step 2 Configuring the toolchain.
  - Modification of lines 4-6 :
    - SET(COMPILER /mnt/fileroot/siqi.yang/C1/toolchain/gcc/linux-x86/aarch64/gcc-linaro-6.3.1-2017.02-x86_64_aarch64-linux-gnu/bin/)
    - SET(CMAKE_C_COMPILER ${COMPILER}/aarch64-linux-gnu-gcc)
    - SET(CMAKE_CXX_COMPILER ${COMPILER}/aarch64-linux-gnu-g++)

- compiling
  - Currently provides scripts to compile all demos with one click!
  - Execute the command:
    - ./make_total.sh
    - After compilation, the executables for each demo will be copied to the directory corresponding to test_case.

# 6、Executable file run

- Take A311X2 for example
  - Push the test_case/A311X2 directory to the platform, e.g. copy it to the /data path
  - cd to each demo directory and execute the following commands to get the results
    - Note: The second parameter xxx.nb/adla is selected according to the board model.

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

# 7、FQA

- If you want to run the example demo directly on the board, you need to do the following
  - C305X&C308X&A311X2 platform driver version is 6.4.11
    - Viewing method: dmesg | grep Galcore
  - C308L platform driver version is 1.3.0
    - Viewing method: dmesg | grep adla
  - Ensure that the nnsdk version is 2.6.4-2.6.6, the current amlnnsdk package provides the 2.6.6 version of the library file
    - Viewing method: strings /usr/lib/libnnsdk.so | grep "NNSDK"