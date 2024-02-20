# 1、Introduction to Catalog Structure

```
├── library                                          --- amlogic and third-party library files and header files
│   ├── 3rdparty
│   │   ├── include
│   │   └── lib
│   └── amlnnsdk
│       ├── include
│       ├── lib
│       └── release.md
├── out                                              --- Compiled demo executable files for each platform
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
├── scripts                                          --- compile script
│   ├── arm32_toolchain.cmake
│   ├── arm64_toolchain.cmake
│   ├── CMakeLists.txt
│   ├── make_nnsdk_32.sh
│   ├── make_nnsdk_64.sh
│   └── make_total.sh
└── source                                           --- Demo source files, models and inputs for each platform
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

- Currently, three demos are provided, namely, classification demo, target detection demo and face demo
- Classification demo
  - Classification demo is usually used to classify images, that is, classify the input image into one or more of the predefined categories. These categories can be cats, dogs, cars, airplanes, etc. The output of the demo is the output of each category. The score refers to the possibility that the image or video frame belongs to each category, while the top5 score refers to the five highest values in the output score, which is usually used to display the most likely categories in the image.
- Detect demo
  - The detection demo is used to detect target objects in images, locate the target's position, and output the bounding box coordinates, category, and confidence of the target. The bounding box coordinates refer to the position information of the target object in the image, the category refers to the category to which the target object belongs, and the confidence level indicates the detector's credibility for the target.
- Face demo
  - The face demo is used to detect faces in images and output the position information of the face, the coordinates of the five key points and the confidence level. The position information includes the coordinates of the bounding box of the face. The coordinates of the five key points usually refer to the eyes. , the location of key parts such as the nose and mouth, and the confidence level indicates how trustworthy the detector is for the face

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

- Modify the AML_NN_DEMO/Linux/scripts folder arm64_toolchain.cmake or arm32_toolchain.cmake file
  - Set the path to gcc-linaro: set it to the directory where the toolchain will be placed after unpacking in step 2 Configuring the toolchain.
  - Modification of lines 4-6 , change the path to your local compilation chain path:
    - SET(COMPILER /mnt/fileroot/xinxin.he/buildroot/toolchain/gcc/linux-x86/arm/gcc-linaro-6.3.1-2017.02-x86_64_arm-linux-gnueabihf/bin/)
    - SET(CMAKE_C_COMPILER ${COMPILER}/arm-linux-gnueabihf-gcc)
    - SET(CMAKE_CXX_COMPILER ${COMPILER}/arm-linux-gnueabihf-g++)

- compiling
  - Currently provides scripts to compile all demos with one click!
  - Execute the command:
    - cd AML_NN_DEMO/Linux/scripts
    - ./make_total.sh
    - After compilation, the executables for each demo will be copied to the directory corresponding to AML_NN_DEMO/Linux/out.

# 6、Executable file run

Customers can create a new directory on the platform specifically to store the files required for demo execution, such as creating a /data/case directory, and then push the executable files, models, and inputs to the /data/case directory on the platform as A311X2 Platform as an example

- Classification demo

  - Open cmd under AML_NN_DEMO/Linux/ and execute the following command

    - push executable file：adb push out/A311X2/image_classify/aml_a311x2_image_classify_32 /data/case
    - push model file：adb push source/A311X2/image_classify/sdk_mobilenetv1_1e_A311X2.nb /data/case
    - push input file：adb push source/A311X2/image_classify/224x224x3_fish.jpeg /data/case

  - After the board is switched to the /data/case directory, execute the following command

    - ./aml_a311x2_image_classify_32 sdk_mobilenetv1_1e_A311X2.nb 224x224x3_fish.jpeg 0 0

    ```
    top 0:score--15.631336,class--2
    top 1:score--11.977257,class--116
    top 2:score--10.048716,class--843
    top 3:score--9.744209,class--444
    top 4:score--9.236698,class--795
    ```

- Face demo

  - Open cmd under AML_NN_DEMO/Linux/ and execute the following command

    - push executable file：adb push out/A311X2/image_classify/aml_a311x2_face_landmark5_32 /data/case
    - push model file：adb push source/A311X2/image_classify/det_retinaface_1e_A311X2.nb /data/case
    - push input file：adb push source/A311X2/image_classify/640x384x3_tfboy.jpg /data/case

  - After the board is switched to the /data/case directory, execute the following command

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

- Detect demo

  - Open cmd under AML_NN_DEMO/Linux/ and execute the following command

    - push executable file：adb push out/A311X2/image_classify/aml_a311x2_object_detect_yolov5s_32 /data/case
    - push model file：adb push source/A311X2/image_classify/yolov5s_1e_A311X2.nb /data/case
    - push input file：adb push source/A311X2/image_classify/640x384x3_tennis.jpg /data/case

  - After the board is switched to the /data/case directory, execute the following command

    - ./aml_a311x2_object_detect_yolov5s_32 yolov5s_1e_A311X2.nb 640x384x3_tennis.jpg 0 0

    ```
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