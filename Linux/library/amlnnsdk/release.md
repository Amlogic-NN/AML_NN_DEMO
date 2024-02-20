```
##########################################################
Release notes for NNSDK_V2.6.4 , 2023.11.17

Change Lists:

New Feature:
    1. Modify aml_read_chip_info logic.
    2. Corrected spelling errors in the header file.
    3. Remove aipq_flag and change to more general logic, which can convert the input according to the model type.
    4. Encapsulate repeated parts into functions.
    5. Add member variables that receive ADLA_COMPILE_ARGS structure information in the aml_compiler_args_t structure to enable external configuration of different compilation options through config.
    6. Modify aml_get_hardware_type logic and add gpu device node.
    7. Align compiler structure.
    8.Increase the upper limit of input and output buf.

Solve bugs:
    1. Fix the bug in allocating space to the output buf in the viplite process.
    2. Fix the bug of memory leak in DMA mode.

##########################################################
Release notes for NNSDK_V2.6.1 , 2023.10.31

Change Lists:

New Feature:
    1. Support the old aipq model of t5m

##########################################################
Release notes for NNSDK_V2.6.0 , 2023.10.20

Change Lists:

New Feature:
    1. Support adla driver_v1.3.0.

Solve bugs:
    1. Fix some aml_util_getProfileInfo bugs for adla driver.

##########################################################
Release notes for NNSDK_V2.5.0 , 2023.09.08

Change Lists:

New Feature:
    1. nnsdk support GPU inference logic.
    2. nnsdk support GPU and NPU parallel inference logic.

##########################################################
Release notes for NNSDK_V2.4.0 , 2023.08.03

Change Lists:

New Feature:
    1. nnsdk support int32/16 input and output type.
    2. nnsdk added the process of Invoke Wait + id.

Solve bugs:
    1. Fix some NCHW_2_NHCW transform bugs for adla driver.
    2. Fix some bind buf bugs for adla driver.

##########################################################
Release notes for NNSDK_V2.3.0 , 2023.03.30

Change Lists:

New Feature:
    1. Support adla driver_v1.2.0.

Solve bugs:
    1. Fix some int16 output bugs for adla driver.
    2. Fix some NHWC_2_NCHW transform bugs for adla driver.
    3. Fix some multi-input and multi-output dma process bugs.

##########################################################
Release notes for NNSDK_V2.2.1 , 2022.11.22

Change Lists:

New Feature:
    1. Support adla driver_v0.10.7.
    
Improvements:
	1. Optimize load model and run graph process.

Solve bugs:
    1. Fix some multi-thread run model stability issues.

##########################################################
Release notes for NNSDK_V2.0.5 , 2022.09.30

Change Lists:

New Feature:
    1. Support adla driver_v0.10.2.
    2. Support get hardware info by nnsdk interface.

Improvements:
	1. Optimize aml_util_swapExternalInputBuffer and aml_util_swapExternalOutputBuffer interface for dma function.
    2. Optimize getouput process.
    3. Optimize get performance interface.

Solve bugs:
    1. Fix some destroy bug.

##########################################################
Release notes for NNSDK_V2.0.0 , 2022.06.30

Change Lists:
New Feature:
    1. Support adla driver_v0.9.

##########################################################
Release notes for NNSDK_V1.9.4 , 2021.12.15

Change Lists:
New Feature:
    1. Support ddk6.4.8.7, it will support older nbg version also.
    2. Support get ddk version by nnsdk interface.
    3. Support save npu input data.

Improvements:
	1. Open more nndemo postprocess source code on github.
    2. Optimize destory process.
    3. Optimize get performance interface.

Solve bugs:

    1. Fix get output bug when type set AML_OUTDATA_RAW.

##########################################################
Release notes for NNSDK_V1.9.2 , 2021.09.30

Change Lists:
New Feature:
    1. Support viplite driver.
    2. Support get ddk version by nnsdk interface.

Improvements:
	1. Open more nndemo postprocess source code on github.
    2. Optimize the pre-processing process

Solve bugs:

    1. Fix some multi_input model verify fail bugs.
    2. Fix some destory bugs

##########################################################
Release notes for NNSDK_V1.8.2 , 2021.06.30

Change Lists:
New Feature:

    1. Support input/output buffer use the physical memory directly.
    2. Support ddk6.4.6.2, it will support older nbg version also.

Improvements:
	1. Open model postprocess source code on github.

Solve bugs:

    1. Fix destory bug when use dma buffer.
	2. Fix some int16 quantize model preprocess bugs
	
##########################################################
Release notes for NNSDK_V1.8.0 , 2021.03.22

Change Lists:
New Feature:

    1. Support input/output buffer use the virtual memory directly.
    2. Add obtain model performance info by external function API.
    3. Add reorder channel control interface.
    4. Add aml_util_swapOutputBuffer interface for dma function.

Improvements:
	1. optimize internal interface to obtain model performance info.
	2. Open get_jpeg_rawData and yolov2 postprocess source code.

Solve bugs:

    1. Fix single ouput model can't get output tensor name by aml_module_output_get way issue.

##########################################################
Release notes for NNSDK_V1.7.2 , 2020.12.25

Change Lists:
New Feature:

   	1.  update rbf face detect demo.
   	2.  Support ddk6.4.4.3, it will support older nbg version also.

Solve bugs:

    1. Some int16 bugs.
    2. Some 32 system bug.

##########################################################
Release notes for NNSDK_V1.7.0 , 2020.8.25

Change Lists:
New Feature:
	1. Support ffmpeg, input can be mp4,avi, flv and so on, the output will be mp4,this will be useful for object demo.
	2. Add rbf face detect demo.
	3. Add android ndk object dectect demo based on picture and usb camera.
	4. Split nnsdk to user library and demo library.

##########################################################
Release notes for NNSDK_V1.6.2 , 2020.8.21

Change Lists:
New Feature:
	1. Add get csutomId and power status by sysfs function API.

##########################################################
Release notes for NNSDK_V1.6.0 , 2020.7.31

Change Lists:
New Feature:
	1. Support ddk6.4.3, modify nbg loading module. It will support older nbg version also.
	2. Support demo based on usb camera , and display on the framebuffer.
	3. Develop the sysfs function API.Set control command to driver by this API
	4. Add profile obtain methods ,include memory, bandwidth and performance information.
	5. Redefine custom netork work flow.
	6. Add head detect demo.

##########################################################
Release notes for NNSDK_V1.5.0 , 2020.6.24

Change Lists:
New Feature:
	1. Code refactoring, Version Compatibility , compatible with all later versions. Remove the data struct dependency.
	2. Main demo code refactoring, just as init, run, destroy.
	3. Add and optimize IO dma flush function.
	4. Add some util function, as get_input_tensor_info and get_output_tensor_info.
	5. Add emotion network demo.
	6. Add person detect network demo.

note: support older version by rebuild use the new headfiles.
###########################################################
Release notes for NNSDK_V1.4.0 , 2020.5.26

Change Lists:
New Feature:
	1. Modify nbg loading module, both support load nbg from memory and file.(v1.3 only support load nbg from file)
	2. Add face 68 key-point network demo.
	3. Add image segmentation network demo.

Solve bugs:
	1. Modify ONNX preprocess bugs.
	2. modify Quantize_None type init bugs.

###########################################################
Release notes for NNSDK_V1.3.0 , 2020.4.22

Change Lists:
New Feature:
	1. support ddk6.4.2, modify nbg loading module. It will support older nbg version also.
	2. The name list can be get for the input and output tensor. Then the user will distinguish the similar tensor from there name.

Solve bugs:
	1. jpeg demo memory leak 
	2. solve some destroy bug,then amlnn_create and  amlnn_destroy can called frequently.

###########################################################
Release notes for NNSDK_V1.2.0 , 2020.3.20

Change Lists:
New Feature:
	1. Support input dma swap feature, as pingpang buffer, then the memcpy opration can be reduce.
	2. Optimize input RGB24 algorithm, then the dtype2float and float2dtype can be reduce, the preprocess time will reduce obvious.
	3. Develop and integrate the face compare network demo. The network demo can compare whether same person of the input two pictures.
	4. Support multi-batch input.

Solve bugs:
	1. Modify Darknet\Pytorch\Tflite preprocess bugs.
	2. Some int16 bugs.

###########################################################
Release notes for NNSDK_V1.1.0 , 2020.2.27

Change Lists:
New Feature:
	1. Support input dma feature.
	2. Support output dma feature.
	3. Opencv micro control, opencv and jpeg can split from sdk by set micro.
	4. Ouput data type config modify, both support rawdata and float32.(v1.0 only support float32)
	5. Add age network demo,modify gender demo output type.

Improvements:
	1.Dtype2float and float2dtype function optimize.

Solve bugs:
	1. Pytorch preprocess function bug.
	2. Input of binary data bug.
	3. Int16 type quantization bug.
###########################################################
```
