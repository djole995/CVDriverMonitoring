#ifndef _VIDEO_PROVIDER_H_
#define _VIDEO_PROVIDER_H_

#include <opencv/cv.h>

/**
 * @brief The VideoProvider abstract class which presents custom video source.
 */
class VideoProvider
{
public:

    /**
     * @brief acquire frame from video source
     *
     * @param frame [out] acquired frame
     *
     * @return true if frame is successfully acquired, false otherwise
     */
    virtual bool GetFrame(cv::Mat& frame)=0;
};








#endif
