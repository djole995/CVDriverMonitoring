project(CVDriverMonitoring)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_STANDARD 14)

#include debug trace logs option
option(DEBUG_LOGS "DEBUG_LOGS" OFF)

#Application developing for AA enviroment
option(USE_ARA_API "USE_ARA_API" OFF)

if(DEBUG_LOGS)
    add_definitions(-DDEBUG_VERSION)
endif()

if(USE_ARA_API)
    add_definitions(-DUSE_ARA_API)
endif()

INCLUDE_DIRECTORIES(include)

aux_source_directory(src SRC_LIST)

add_executable(${PROJECT_NAME} ${SRC_LIST})

set(CV_LIB_FOLDER /usr/local/lib)

target_link_libraries(${PROJECT_NAME}
    ${CV_LIB_FOLDER}/libopencv_core.so
    ${CV_LIB_FOLDER}/libopencv_highgui.so
    ${CV_LIB_FOLDER}/libopencv_imgproc.so
    ${CV_LIB_FOLDER}/libopencv_objdetect.so
    ${CV_LIB_FOLDER}/libopencv_videoio.so)

