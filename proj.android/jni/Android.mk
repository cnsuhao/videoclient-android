LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := cocos2dcpp_shared

LOCAL_MODULE_FILENAME := libcocos2dcpp

LOCAL_SRC_FILES := hellocpp/main.cpp \
                   ../../Classes/AppDelegate.cpp \
                   ../../Classes/HelloWorldScene.cpp \
				   ../../Classes/CCGameScrollView.cpp \
				   ../../Classes/CCGameScrollViewDelegate.cpp \
				   ../../Classes/FileFilter.cpp \
				   ../../Classes/Common.cpp \
				   ../../Classes/MediaPlayScene.cpp \
				   ../../Classes/xhDecodec.cpp \
				   ../../Classes/xhThreadSafe.cpp \
				   ../../Classes/xhMediaFile.cpp \
				   ../../Classes/MediaPlayer.cpp \
				   ../../Classes/MediaAudio.cpp \				   

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../Classes

LOCAL_WHOLE_STATIC_LIBRARIES += cocos2dx_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocosdenshion_static
LOCAL_WHOLE_STATIC_LIBRARIES += box2d_static
LOCAL_WHOLE_STATIC_LIBRARIES += chipmunk_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocos_extension_static
LOCAL_WHOLE_STATIC_LIBRARIES += boost_thread
LOCAL_WHOLE_STATIC_LIBRARIES += boost_filesystem
LOCAL_WHOLE_STATIC_LIBRARIES += boost_system
LOCAL_SHARED_LIBRARIES += ffmpeg_share
LOCAL_SHARED_LIBRARIES += libopenal_share
LOCAL_SHARED_LIBRARIES += libiconv

include $(BUILD_SHARED_LIBRARY)

$(call import-module,openal)
$(call import-module,ffmpeg/armv6)
$(call import-module,libiconv-1.8)
$(call import-module,boost_1_53)
$(call import-module,cocos2dx)
$(call import-module,cocos2dx/platform/third_party/android/prebuilt/libcurl)
$(call import-module,CocosDenshion/android)
$(call import-module,extensions)
$(call import-module,external/Box2D)
$(call import-module,external/chipmunk)
