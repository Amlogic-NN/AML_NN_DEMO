# image_classification

Model-Type: uint8

Description: continuously classify whatever it sees from the device's back camera

Include the model of:efficientnet_lite0_fp32_2_1.tflite  efficientnet_lite0_int8_2.tflite  mobilenet_v1_1.0_224_1_metadata_1.tflite  mobilenet_v1_1.0_224_quantized_1_metadata_1.tflite 

Overview link: https://tensorflow.google.cn/lite/examples/image_classification/overview

platform: A311D2

system: Android T

adla_soft version: 1.3.0

user guide:

​    adb install -t Image_Classify_nnapi_baseon_camera.apk

​	plug in the camera to the board

​    give camera permission after installing apk