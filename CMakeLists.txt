project(CVDriverMonitoring)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_STANDARD 14)

add_definitions(-DHAS_VSOMEIP_BINDING)

#include debug trace logs option
option(DEBUG_LOGS "DEBUG_LOGS" OFF)

#Application developing for AA enviroment
option(USE_ARA_API "USE_ARA_API" OFF)

#Application developing for AA enviroment
option(TARGET_QCM "USE_ARA_API" OFF)

INCLUDE_DIRECTORIES(include ../common/DisplayAPI/inc gen/includes gen/vsomeip)

aux_source_directory(src SRC_LIST)
aux_source_directory(../common/DisplayAPI/inc SRC_LIST)

if(DEBUG_LOGS)
    add_definitions(-DDEBUG_VERSION)
endif()

if(USE_ARA_API)
    add_definitions(-DUSE_ARA_API)
endif()

if(TARGET_QCM)
    message("QCM build")
    add_definitions(-DTARGET_QCM)
    set(CMAKE_MODULE_PATH ${DENSO_TOOLCHAIN_PATH}/sysroots/8x96auto/usr/share/cmake-3.4/Modules)
    set(CMAKE_CXX_COMPILER ${DENSO_TOOLCHAIN_PATH}/sysroots/x86_64-linux/usr/bin/aarch64-agl-linux/aarch64-agl-linux-g++)
    set(CMAKE_C_COMPILER ${DENSO_TOOLCHAIN_PATH}/sysroots/x86_64-linux/usr/bin/aarch64-agl-linux/aarch64-agl-linux-gcc)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 --sysroot=${DENSO_TOOLCHAIN_PATH}/sysroots/8x96auto")

    aux_source_directory(src/qcm SRC_LIST)

    aux_source_directory(gen SRC_LIST)
    aux_source_directory(gen/vsomeip SRC_LIST)


    add_executable(${PROJECT_NAME} ${SRC_LIST})

    target_link_libraries(${PROJECT_NAME} pthread CameraInput
    libopencv_core.so
    libopencv_highgui.so
    libopencv_imgproc.so
    libopencv_objdetect.so
    libopencv_videoio.so
wayland-client wayland-egl EGL dl GLESv2 DisplayAPI
    AdaptivePlatform
    ARAExec
    ara_vsomeipbinding
    ara_e2ebinding
    ara_com
    vsomeip
    keyvaluestorage
    E2EXF)
else()
    message("Linux build")
    add_executable(${PROJECT_NAME} ${SRC_LIST})
    target_link_libraries(${PROJECT_NAME}
    /usr/local/lib/libopencv_core.so
    /usr/local/lib/libopencv_highgui.so
    /usr/local/lib/libopencv_imgproc.so
    /usr/local/lib/libopencv_objdetect.so
    /usr/local/lib/libopencv_videoio.so
wayland-client wayland-egl EGL dl GLESv2 DisplayAPI)
endif()





