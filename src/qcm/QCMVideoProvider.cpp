#include "qcm/QCMVideoProvider.h"
#include "DebugUtils.h"

#include <opencv2/imgproc.hpp>

const char* frame_dir = "/home/djokicm/RT-RK/vm-shared-dir/frames";

bool QCMVideoProvider::Init(CAM_Cameras camera_id)
{
    CAM_Error cerr;

    // Open and start camera
    cerr = CAM_Open(camera_id, 1);
    if(cerr != CAM_OK)
    {
        DEBUG_LOG("Unable to open cameras.");
        return false;
    }

    cerr = CAM_Start(1);
    if(cerr != CAM_OK)
    {
        DEBUG_LOG("Unable to start cameras.");
        return false;
    }

    camera_id_ = camera_id;

    return true;
}

bool QCMVideoProvider::Shutdown()
{
    CAM_Error cerr;

    // Free camera resources
    cerr = CAM_Stop(1);
    if(cerr != CAM_OK)
    {
        DEBUG_LOG("Unable to stop cameras.");
        return false;
    }

    cerr = CAM_Close(camera_id_, 1);
    if(cerr != CAM_OK)
    {
        DEBUG_LOG("Unable to close cameras.");
        return false;
    }

    return true;
}

bool QCMVideoProvider::GetFrame(cv::Mat& frame)
{
    size_t sz;
    void* ptr;
    CAM_Error cerr;

    cerr = CAM_GetFrame(&ptr, &sz, 0);
    if(cerr != CAM_OK)
    {
        DEBUG_LOG("Failed to get camera frame!");
        return false;
    }

    memcpy(frame_, ptr, QCM_CAM_FRAME_SIZE);
    frame = cv::Mat(QCM_CAM_FRAME_HEIGHT, QCM_CAM_FRAME_WIDTH, CV_8UC2, frame_);

    cerr = CAM_ReleaseFrame(&ptr, 0);
    if(cerr != CAM_OK)
    {
        DEBUG_LOG("Failed to release camera frame!");
        return false;
    }


    return true;
}
