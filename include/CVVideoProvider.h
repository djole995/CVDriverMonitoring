#ifndef _CV_VIDEO_PROVIDER_H_
#define _CV_VIDEO_PROVIDER_H_

#include "VideoProvider.h"
#include <opencv2/videoio.hpp>

class CVVideoProvider : public VideoProvider
{
public:

    CVVideoProvider() = default;

    ~CVVideoProvider() = default;

    bool Init() override;

    bool GetFrame(cv::Mat& frame) override;

    void Shutdown() override;

private:

    cv::VideoCapture video_capture_;

};


#endif
