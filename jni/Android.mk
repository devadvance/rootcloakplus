LOCAL_PATH := $(call my-dir) 

include $(CLEAR_VARS)
LOCAL_MODULE := substrate
LOCAL_SRC_FILES := C:\Users\Matt\android-sdk\extras\saurikit\cydia_substrate\lib\armeabi\libsubstrate.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := substrate-dvm
LOCAL_SRC_FILES := C:\Users\Matt\android-sdk\extras\saurikit\cydia_substrate\lib\armeabi\libsubstrate-dvm.so
include $(PREBUILT_SHARED_LIBRARY)
 
include $(CLEAR_VARS)  
LOCAL_LDLIBS := -llog  -lsubstrate -lsubstrate-dvm
LOCAL_SHARED_LIBARIES := substrate substrate-dvm
LOCAL_MODULE    := rootcloakplus.cy
LOCAL_SRC_FILES := rootcloakplus.cy.cpp 
LOCAL_CPPFLAGS := -fpermissive
include $(BUILD_SHARED_LIBRARY) 