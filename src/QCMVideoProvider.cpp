#include "QCMVideoProvider.h"
#include "DebugUtils.h"

#include <opencv2/imgproc.hpp>

const char* frame_dir = "/home/djokicm/RT-RK/vm-shared-dir/frames";

static QCMVideoProvider* instance_ptr;

void FrameArrived(uint8_t* new_frame, int stream_id)
{
    instance_ptr->frame_lock.lock();

    memcpy(instance_ptr->frame_, new_frame, QCM_CAM_FRAME_SIZE);

    instance_ptr->frame_lock.unlock();
}

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

    CAM_RegisterFrameCB(FrameArrived, NULL);

    cerr = CAM_Start(1);
    if(cerr != CAM_OK)
    {
        DEBUG_LOG("Unable to start cameras.");
        return false;
    }

    camera_id_ = camera_id;

    instance_ptr = this;

    return true;
}

bool QCMVideoProvider::Shutdown()
{
    CAM_Error cerr;

    // Free camera resources
    cerr = CAM_Stop(1);
    if(cerr != CAM_OK)
    {
        std::cout << "Unable to stop cameras." << std::endl;
        return false;
    }

    cerr = CAM_Close(camera_id_, 1);
    if(cerr != CAM_OK)
    {
        std::cout << "Unable to close cameras." << std::endl;
        return false;
    }

    return true;
}

bool QCMVideoProvider::GetFrame(cv::Mat &frame)
{
    frame = cv::Mat(QCM_CAM_FRAME_HEIGHT, QCM_CAM_FRAME_WIDTH, CV_8UC2, frame_);

    cv::cvtColor(frame, frame, CV_YUV2BGR_UYVY);

    return true;
}
