#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_

#include <cstddef>


constexpr size_t TARGET_RESOLUTION_WIDTH = 192;
constexpr size_t TARGET_RESOLUTION_HEIGHT = 168;

// @note Set specific resolution for input test video, this will be changed when camera on qcm is used for input.
constexpr size_t INPUT_RESOLUTION_WIDTH = 1280;
constexpr size_t INPUT_RESOLUTION_HEIGHT = 720;

// @note Hardcoded crop values for used input video
constexpr size_t HORIZONTAL_CROP = (size_t)(INPUT_RESOLUTION_WIDTH -
        ((double)TARGET_RESOLUTION_WIDTH / TARGET_RESOLUTION_HEIGHT) *
        INPUT_RESOLUTION_HEIGHT + .5) >> 1;
constexpr size_t VERTICAL_CROP = 0;

//DROWSINESS SCORE
#define driver_drowsiness_FPS (30.0f)

#define driver_drowsiness_Min_Score (0.0f)
#define driver_drowsiness_Low_Limit (3.0f)
#define driver_drowsiness_Medium_Limit (5.0f)
#define driver_drowsiness_Max_Score (10.0f)

#define driver_drowsiness_Low_Price (1.5f)
#define driver_drowsiness_Medium_Price (3.0f)
#define driver_drowsiness_High_Price (4.0f)

#endif
