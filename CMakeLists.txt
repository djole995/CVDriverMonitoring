project(CVDriverMonitoring)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_STANDARD 14)

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

