# face detect 

Model-Type: uint8

Description: continuously detects the face in the frames seen by your device's camera

Include the model of: det_retinaface_int8_uint8.tflite

Apk link:  https://github.com/Amlogic-NN/AML_NN_DEMO/tree/main/Android/Amlogic/face_detect_v4/[face_detect_v4.apk](https://github.com/Amlogic-NN/AML_NN_DEMO/blob/main/Android/Amlogic/face_detect_v4/face_detect_v4.apk)

Source code link:https://github.com/Amlogic-NN/AML_NN_DEMO/tree/main/Android/Amlogic/face_detect_v4/face_detect_v4.zip

platform: A311D2

system: Android R

adla_soft version: 1.3.0

user guide:

​    adb install -t face_detect.apk

​	plug in the camera to the board

​    Give camera permission after installing apk

​    If your device has TFL_Detect apk ,first uninstall the TFL_Detect apk and then install this apk.

**PS:** 

​    **Face detection already includes age detection and gender detection**



# age detection

Model-Type: uint8

Description: continuously detects the face age in the frames seen by your device's camera

Include the model of: sdk_age_quant.acuity.tflite

platform: A311D2

system: Android R

adla_soft version: 1.3.0



# gender detection

Model-Type: uint8

Description: continuously detects the face gender in the frames seen by your device's camera

Include the model of: sdk_gender_int8_uint8.tflite

platform: A311D2

system: Android R

adla_soft version: 1.3.0