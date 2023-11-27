1、directory structure
    example：sample source code
    nnsdk_vx.x.x：nnsdk_vx.x.x dynamic library and header files
    model_convert：model convert example script
    case：example executable and model files

2. Model convert process
     The model_convert directory contains the original model, convert script
     convert process:
         1. Get the release toolkit, take adla-toolkit-binary-x.x.x.x as an example
         2. Replace convert_adla.sh in model_convert with convert_adla.sh in adla-toolkit-binary-x.x.x.x/demo/
         3. Copy sdk_mobilenetv1.pb in model_convert to adla-toolkit-binary-x.x.x.x/demo/model_source/tensorflow_model
         4. ./convert_adla.sh
         5. The converted adla model file is stored in the adla-toolkit-binary-x.x.x.x/demo/tensorflow_output path
     Note:For the process of model conversion, the configuration of input and output and quantization parameters, please refer to <<Model Conversion User Guide (x.x).pdf>>

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
		 ./case_face_det_linux_32 det_retinaface_int8.adla 640x384x3_i8.bin
	3、result：
		face_detNum = 3
		face_number:0, x:259.500000, y:48.500000, w:104.000000, h:142.000000, prob:0.992188
		face_number:1, x:532.500000, y:50.500000, w:102.000000, h:138.000000, prob:0.988281
		face_number:2, x:3.500000, y:71.000000, w:102.000000, h:129.000000, prob:0.988281