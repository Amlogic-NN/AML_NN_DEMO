1. directory structure
     example: sample source code
     nnsdk_vx.x.x: nnsdk_vx.x.x dynamic library and header files
     model_convert: model convert example script
     case: example executable and model files

2. Model convert process
     The model_convert directory contains the original model, convert script
     convert process:
         1. Get the release toolkit, take adla-toolkit-binary-x.x.x.x as an example
         2. Replace convert_adla.sh in model_convert with convert_adla.sh in adla-toolkit-binary-x.x.x.x/demo/
         3. Copy sdk_mobilenetv1.pb in model_convert to adla-toolkit-binary-x.x.x.x/demo/model_source/tensorflow_model
         4. ./convert_adla.sh
         5. The converted adla model file is stored in the adla-toolkit-binary-x.x.x.x/demo/tensorflow_output path
     Note:For the process of model conversion, the configuration of input and output and quantization parameters, please refer to <<Model Conversion User Guide (x.x).pdf>>


3. configure the ndk compilation environment：
	2.1 Downloadandroid-ndk-r21e
		https://github.com/android/ndk/wiki/Unsupported-Downloads
	2.2 unzip
	2.3 Set environment variables
        export NDKROOT=/home/siqiyang/work/ndk/android-ndk-r21e
        export PATH=$NDKROOT:$PATH

4. ndk compilation
	Execute ndk-build in the example path, and the executable file will be saved in the example\obj\local\armeabi-v8 path

5. executable file running
	5.1 Copy the entire current directory to the /vendor directory
	5.2 Execute the command（argv[1] is the executable file name, argv[2] is the adla model name, argv[3] is the input name, the following takes the a311d2 platform android 64-bit environment as an example）：
		./case_face_recog_android_64 det_insightface_int8.adla 112x112x3_i8.bin
		print output buf, which is a 512-dimensional vector
