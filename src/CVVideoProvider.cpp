#include "CVVideoProvider.h"
#include <opencv2/opencv.hpp>

bool CVVideoProvider::Init(const std::string& file_name)
{
    // using web camera as video source
    if(file_name == "0")
    {
        if(!video_capture_.open(0))
        {
            return false;
        }
        else
        {
            return true;
        }
    }
    // using file as video source
    else
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
