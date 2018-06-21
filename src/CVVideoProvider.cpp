#include "CVVideoProvider.h"

bool CVVideoProvider::Init(const std::string& file_name)
{
    if(!video_capture_.open(file_name))
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
