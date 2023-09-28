LOCAL_PATH := $(call my-dir)
LIB_PATH := $(LOCAL_PATH)/../../nnsdk_v2.5.0/lib/lib64

$(warning $(LOCAL_PATH))

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	main.c \
	postprocess_util.c

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/../../nnsdk_v2.5.0/include \
	$(LOCAL_PATH)/../../nnsdk_v2.5.0/include/jpeg

LOCAL_LDFLAGS := -L$(LIB_PATH) -lnnsdk -ljpeg_t

LOCAL_LDLIBS += -llog -ldl -lm
LOCAL_LDLIBS += -fuse-ld=gold

LOCAL_MODULE         := case_face_recog_android_64

include $(BUILD_EXECUTABLE)