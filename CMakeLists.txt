project(CVDriverMonitoring)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_STANDARD 14)

#include debug trace logs option
option(DEBUG_LOGS "DEBUG_LOGS" OFF)

#Application developing for AA enviroment
option(USE_ARA_API "USE_ARA_API" OFF)

#Application developing for AA enviroment
option(TARGET_QCM "USE_ARA_API" OFF)

if(DEBUG_LOGS)
    add_definitions(-DDEBUG_VERSION)
endif()

if(USE_ARA_API)
    add_definitions(-DUSE_ARA_API)
endif()

if(TARGET_QCM)
    message("QCM build")
    set(CMAKE_MODULE_PATH ${DENSO_TOOLCHAIN_PATH}/sysroots/8x96auto/usr/share/cmake-3.4/Modules)
    set(CMAKE_CXX_COMPILER ${DENSO_TOOLCHAIN_PATH}/sysroots/x86_64-linux/usr/bin/aarch64-agl-linux/aarch64-agl-linux-g++)
    set(CMAKE_C_COMPILER ${DENSO_TOOLCHAIN_PATH}/sysroots/x86_64-linux/usr/bin/aarch64-agl-linux/aarch64-agl-linux-gcc)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 --sysroot=${DENSO_TOOLCHAIN_PATH}/sysroots/8x96auto")
endif()

INCLUDE_DIRECTORIES(include)

aux_source_directory(src SRC_LIST)

add_executable(${PROJECT_NAME} ${SRC_LIST})

target_link_libraries(${PROJECT_NAME} pthread CameraInput
    libopencv_core.so
    libopencv_highgui.so
    libopencv_imgproc.so
    libopencv_objdetect.so
    libopencv_videoio.so)

