1、directory structure
    example：sample source code
    nnsdk_vx.x.x：nnsdk_vx.x.x dynamic library and header files
    model_convert：model convert example script
    case：example executable and model files

2. Model convert process
     The model_convert directory contains the original model, convert script
     convert process:
         1. Get the release toolkit, take acuity-toolkit-binary-x.x.x.x as an example
         2. Copy the entire model_convert to the same path as the demo in the toolkit, and then execute the script in sequence
         3. ./cmd_import.sh	./cmd_quantize.sh           ./cmd_export.sh 0xbe
         4. The converted nb model file is stored in the det_yolov2_0xbe path
     Note:For the process of model conversion, the configuration of input and output and quantization parameters, please refer to <<Model Transcoding and Running User Guide (x.x).pdf>>

3、configure the cmake compilation environment：
	1、download cmake
		https://cmake.org/download/
		Note: The version must be higher than 3.5.1
	2、 install cmake
		tar -xvf cmake-3.*.*.tar
		cd cmake-3.*.*
		./bootstrap 
		make
		make install
	3、 verify successful installation
		cmake --version
		displaying the cmake version is successful

4、configure compilation tool chain

5、executable file compilation
	1、revise CMakeLists.txt
		A、revise GCC_PATH：
			revise line 5 :SET(GCC_PATH /xxx/xxx)
			set the gcc-linaro path: set it to the second step of configuring the tool chain, and put the tool chain in the directory after decompression.
			for example, the path placed on my computer is:/mnt/fileroot/xinxin.he/buildroot/toolchain/gcc/linux-x86
			i.e. set：set(GCC_PATH /mnt/fileroot/xinxin.he/buildroot/toolchain/gcc/linux-x86)

		B、compile arm32 executable files by default：
		Note: If you build your own project, you can copy the CMakeLists.txt file to the same directory as the main function of the project

	2、compile
		execute the command under the nnsdk path：
		A、cmake .    //generate makefile from current path
		B、make      //compile
		the generated executable file is in the current directory
		C、copy the executable file to the case directory

6、executable file running
	1、Copy the entire current directory to the / directory
	2、Execute the command：
		 ./case_obj_det_linux_64 det_yolov2_0xbe.nb space_shuttle_416x416_u8_nchw.bin
	3、result：
                             obj_number:0,class:airplane,label num:4,prob:0.600094
                             left:-18.808651,right:387.168945,top:21.773304,bot:282.587006