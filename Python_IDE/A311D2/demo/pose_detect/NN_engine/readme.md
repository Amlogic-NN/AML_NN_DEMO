目录结构：
├── nnengine_pc_37
│   └── amlnn	                            #Windows上nnEngine pyd包
│       ├── engine.cp37-win_amd64.pyd
│       ├── enginennlog.cp37-win_amd64.pyd
│       ├── enginesock.cp37-win_amd64.pyd
│       └── visual_script.cp37-win_amd64.pyd
│
├── nnengine_board 							#板端nnEngine运行程序
│   ├── android_32
│   │   ├── libjpeg_t.so
│   │   └── nnEngine
│   ├── android_64
│   │   ├── libjpeg_t.so
│   │   └── nnEngine
│   ├── linux_32
│   │   ├── libjpeg.so
│   │   └── nnEngine
│   └── yocto_64
│       ├── libjpeg.so
│       └── nnEngine
├── demo
│   ├── adla_model_file					#转出的不同平台对应的demo adla文件
│   ├── data
│   │   ├── aml_dataset_crop_inception	#测试集图片
│   │   ├── aml_val_4901_inception.txt  #测试集以及标签文件
│   │   ├── dataset.txt					#模型转换时量化数据集文件
│   │   └── input_file					#模型输入数据文件
│   ├── demo_multi_io.py				#多输入多输出模型demo
│   ├── demo.py							#单输入单输出模型，多帧处理demo
│   ├── demo_single.py					#单输入单输出模型，单帧处理demo
│   └── model							#demo 目录目录
│       ├── model_convert.sh			#模型转换脚本
│       ├── check_input_param.sh		#确认输入输出量化参数脚本
│       └── resnet18-v1-7.onnx			#onnx模型


简介：
	nnEngine是Amlogic开发的Windows上使用的一套基于nnsdk开发包的模型推理引擎，利用ADB协议实现嵌入式设备端和服务器端的数据传输和模型运行控制。用户可通过提供的aml_nn_engine python库实现模型推理和性能指标获取功能。
	
整体使用流程介绍：
	1、模型转换
	2、环境配置
	3、nn_engine使用介绍

1、模型转换以及参数确认：
	1. 模型转换
		在linux环境下，将nnengine_demo/model下的模型转成当前使用单板对应的adla文件，使用model_convert.sh	脚本进行转换 	
		Note：记录当前使用的channel-mean-value 127.5,127.5,127.5,127.5
	2. 参数确认
		转出adla文件后，使用check_input_param.sh确认出adla文件对应的输入输出的zero_point/scale
		Note：记录adla文件的input_zero_point/input_scale
	Note：model_convert.sh/check_input_param.sh 在Ubuntu环境执行，且需自己修改配置模型转换工具路径。

2、环境配置：
	1. Windows PC环境配置：
		直接使用nnengine_pc_37中的pyd包即可，在demo中指定好路径，如sys.path.append('../nnengine_pc_37')
	2. 板端环境配置
		1.将nnEngine和adla文件 push到板端/data/nn/nnEngine 目录下
			adb root
			adb remount
			adb shell mkdir /data/nn/
			adb push   /xxxx/xxx.adla  /data/nn/
		2.启动板端server： 
			adb shell
			cd  /data/nn/
			chmod +x nnEngine
			./nnEngine  
			
			Note：
			1.如果启动nnEngine时提示缺libjpeg_t.so时，将nnEngine相同目录下的so push到板端。 
				Android 64bit： /vendor/lib64     Android 32bit： /vendor/lib32
				Linux 64bit：   /lib64            Linux 32bit：   /lib32
			2.如同提示缺libnnsdk.so 
				一般出现在Android环境下，建议将nnEngine push到/vendor 目录下，再启动
			Result：cmd窗口出现如下字样表明，板端server已启动
				server socket begin accept:

3、nn_engine使用介绍 

	1.调用流程以及参数介绍：
		++++++++++++++++++++++++++++++++++++++
		def get_image(path):
			img = Image.open(path).convert('RGB').resize((224,224))
			img=np.array(img)
			img=(img-127.5)/127.5                         #channel-mean-value ，The value recorded when the model is converted
		
			data=np.expand_dims(img,axis=0)
			data=data/0.007843137718737125 +1             #the input zero_point and scale，The value recorded when the model is converted
			data=np.array(data).astype(np.int8)

			return data
		++++++++++++++++++++++++++++++++++++++
		from amlnn import engine
		
		amlnn = engine.AMLNN()
		amlnn.setloglevel("DEBUG")                     # Valid values['DEBUG','INFO','WARNING],default:INFO
		
		#set init config
		config=amlnn.get_config()
		config.device_type = 'android'                # Valid values['android','linux'],defaul:'android'
		config.model_type = 'adla'                    # framework type, ['adla','tflite']
		config.board_path='/data/vendor/nn'           # Working path on the board
		config.nb_path = './resnet18-v1-7_int8.adla'  # The adla file path in the board: /data/nn/resnet18-v1-7_int8.adla

		config.profile = 'all'                        # default: all, Valid values['0','bw','fps','acc','all'], if set value all and setloglevel 'DEBUG', bw and fps information will be printed in the log.
		config.hardware_info = '0'                    # default: all, Valid values['0','all'], if set value all and setloglevel 'DEBUG', Chip information will be printed in the log
		#set init config
		amlnn.init()
		
		
		#set input config
		img_name='./data/input/2242243.jpeg'
		config.input_size='224 224 3'              # input shape info
		config.input_type='raw'                    # input_type info, default:raw, Valid values['raw', 'dma'], Both require input_data to provide quantified data, and the corresponding input copy times are different.
		config.output_type='fp32'                  # output_type info, default:fp32, Valid values['fp32','raw'], raw: Quantitative data, fp32: float32 result
		config.run_cyc=10                          # default: 1,  Call inference, internal execution times.Used to count single frame inference time
		config.input_data = get_image(img_name)    # set_input from memory
		#config.input_data = "./2242243.bin"       # set_input from file,   you need push the  2242243.bin file to the path: /data/nn/2242243.bin  on board
		#set input get_config
		amlnn.set_input()

		#inference
		out=amlnn.inference()
		
		#amlnn.visualize()                         #if you call amlnn.visualize(), the config config.profile need set 'bw' or 'all'. 生成可视化文件，'runtime.jpg'&'bandwidth.jpg'

		amlnn.destroy()
		++++++++++++++++++++++++++++++++++++++

	Note：
		1. PC端执行nnengine_demo目录下demo.py/demo_single.py/demo_multi_io.py 时可能会涉及到一些依赖模块，自己配置下即可。
		2. 多输入多输出模型的input设置以及output获取，请参考demo_multi_io.py
		3. 测试一个数据集，请参考demo.py
	
	