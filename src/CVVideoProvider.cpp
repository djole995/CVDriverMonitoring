#include "CVVideoProvider.h"

static std::string fileName("/home/djokicm/RT-RK/vm-shared-dir/frames/e.mp4");

bool CVVideoProvider::Init()
{
    if(!video_capture_.open(fileName))
    {
        return false;
    }
    else
    {
        return true;
    }
}


bool CVVideoProvider::GetFrame(cv::Mat& frame)
{
    if(!video_capture_.read(frame))
    {
        video_capture_.release();
        return false;
    }
    else
    {
        return true;
    }
}


void CVVideoProvider::Shutdown()
{
    video_capture_.release();
}
