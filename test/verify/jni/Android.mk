LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm
LOCAL_CFLAGS := -g --all-warnings --extra-warnings

# static link
LOCAL_LDFLAGS := -static

LOCAL_MODULE    := test
LOCAL_SRC_FILES := test.c

# Don't stip
cmd-strip := 

include $(BUILD_EXECUTABLE)
