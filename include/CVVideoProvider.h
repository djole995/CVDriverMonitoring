#ifndef _CV_VIDEO_PROVIDER_H_
#define _CV_VIDEO_PROVIDER_H_

#include "VideoProvider.h"
#include <opencv2/videoio.hpp>

/**
 * @brief class which provides video stream from file using OpenCV Video I/O API
 */
class CVVideoProvider : public VideoProvider
{
public:

    /**
     * @brief Constructor
     */
    CVVideoProvider() = default;

    /**
     * @brief Destructor
     */
    ~CVVideoProvider() = default;

    /**
     * @brief load video stream
     *
     * @param file_name video file path
     *
     * @return true on success, false otherwise
     */
    bool Init(const std::string& file_name);

    /**
     * @brief acquire frame from video source
     *
     * @param frame [out] acquired frame
     *
     * @return true if frame is successfully acquired, false otherwise
     */
    bool GetFrame(cv::Mat& frame) override;

    /**
     * @brief cleanup object resources
     */
    void Shutdown();

private:

    cv::VideoCapture video_capture_;  /**< video stream interface */

};


#endif
