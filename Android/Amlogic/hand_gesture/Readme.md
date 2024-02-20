# hand detect

Model-Type:uint8

Description:continuously detects the hand in the frames seen by your device's camera and show hand key points

Include the model of :hand_landmark_full_unt8.tflite

Apk link:https://github.com/Amlogic-NN/AML_NN_DEMO/tree/main/Android/Native/hand_gesture/hand_landmarks.apk

Source code link:https://github.com/Amlogic-NN/AML_NN_DEMO/tree/main/Android/Native/hand_gesture/hand_gesture.zip

platform: A311D2

system: Android T

adla_soft version: 1.3.0

user guide:

​    adb install -t hand_gesture.apk

​	plug in the camera to the board

​    Give camera permission after installing apk

​    If your device has TFL_Detect apk ,first uninstall the TFL_Detect apk and then install this apk.