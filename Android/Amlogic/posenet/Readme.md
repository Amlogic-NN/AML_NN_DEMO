# pose_estimation

Model-Type: int8

Description: continuously detects the body parts in the frames seen by your device's camera

Include the model of: posenet_mobilenet_v1_100_257x257_multi_kpt_stripped.tflite(Supported) yoga_classifier.tflite(Not Supported) lite-model_movenet_multipose_lightning_tflite_float16_1.tflite(Not Supported)

Overview link: https://tensorflow.google.cn/lite/examples/pose_estimation/overview

platform: A311D2

system: Android R

adla_soft version: 1.3.0

user guide:

​    adb install -t posenet.apk

​	plug in the camera to the board

​    Give camera permission after installing apk