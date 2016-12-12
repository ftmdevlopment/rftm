# Copyright (C) 2007 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

LOCAL_PATH := $(call my-dir)

C_ROOT := $(LOCAL_PATH)

TARGET_USE_ADF := false
TARGET_USE_DOUBLE_BUFFER := false
TARGET_RECOVERY_PIXEL_FORMAT := BGRA_8888

include $(CLEAR_VARS)

LOCAL_MODULE := rftm
LOCAL_FORCE_STATIC_EXECUTABLE := true
LOCAL_MODULE_TAGS := tests
LOCAL_CFLAGS += -Wall -std=c++11
LOCAL_SRC_FILES := \
    ui_base.cpp \
    ui_main.cpp \
    ui_test.cpp \
    utils.cpp \
    main.cpp \
    log.cpp \
    qrcode/BitBuffer.cpp \
    qrcode/QrSegment.cpp \
    qrcode/QrCode.cpp \
    test/draw_test.cpp \
    test/triangle_test.cpp \
    test/version_test.cpp \
    test/fan_test.cpp \
    test/key_test.cpp \
    test/led_test.cpp \
    test/light_sensor_test.cpp \
    test/speaker_test.cpp \
    test/dlp_test.cpp \
    test/mem_test.cpp \
    test/wifi_test.cpp \
    test/battery_test.cpp \
    test/clear_flash.cpp \
    test/power_off.cpp \
    test/ftm_exit.cpp \

LOCAL_CPP_FEATURES := exceptions

LOCAL_STATIC_LIBRARIES := \
    libminui \
    libpng \
    libz \
    libstdc++ \
    libc

LOCAL_LDLIBS    := -llog

include $(BUILD_EXECUTABLE)


include $(LOCAL_PATH)/minui/Android.mk \
    $(LOCAL_PATH)/libpng/Android.mk \
    $(LOCAL_PATH)/zlib/Android.mk \

ifeq ($(TARGET_USE_ADF),true)
    $(LOCAL_PATH)/adf/Android.mk
endif
