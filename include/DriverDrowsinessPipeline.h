#ifndef _DRIVER_MONITORING_PIPELINE_
#define _DRIVER_MONITORING_PIPELINE_

#include <opencv2/objdetect.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv/cv.h>
#include <memory>

#include "VideoProvider.h"


/**
 * @brief structure which contains algorithm results
 */
typedef struct DriverMonitoringData_
{
    cv::Mat original_img_;  /**< original input frame */
    cv::Rect face_region_;  /**< detected face region */
    cv::Rect left_eye_region_;  /**< left eye region calculated based on face region */
    cv::Rect right_eye_region_;  /**< right eye region calculated based on face region */
    bool face_found_;  /**< Indicates whether drivers face is found on input frame. */
    bool eyes_found_;  /**< Indicates whether eyes are open or closed */

} DriverMonitoringData;


/**
 * @breif Class which implements driver monitoring algorithm pipeline
 */
class DriverMonitoringPipeline
{
public:

    /**
     * @brief Constructor
     */
    DriverMonitoringPipeline() = default;

    /**
     * @brief Destructor
     */
    ~DriverMonitoringPipeline() = default;

    /**
     * @brief Init pipeline
     * @param face_cascade_name name of face clasificator cascade file
     * @param eyes_cascade_name
     * @return true on success, else false
     */
    bool Init(const std::string& face_cascade_name, const std::string& eyes_cascade_name);

    /**
     * @brief Run algorithm loop, which runs pipeline phases for every new frame
     */
    void Run(VideoProvider& video_provider);

    /**
     * @brief Shutdown cleanup object resources
     */
    void Shutdown();

    /**
     * @brief GetDrowsinessScore
     * @return driver drowisness score
     */
    float GetDrowsinessScore();

private:

    /**
     * @brief Grayscale & crop input image
     */
    void GrayscaleCropImage();

    /**
     * @brief Downscale input image to use it as face detection input
     */
    void DownscaleImage();

    /**
     * @brief Use classifier to detect if drivers frontal face is present on input image
     */
    void PerformFaceDetection();

    /**
     * @brief Use classifier to detect whether eyes ara open or closed
     */
    void PerformEyesDetection();

    /**
     * @brief Update driver drowsiness score based on algorithm detection phases results
     */
    void UpdateScore();

    /**
     * @brief Perform algorithm visualisation by drawing face and eyes rectangles and drowsiness scorebar
     */
    void RenderGraphics();

    DriverMonitoringData data_;  /**< algorithm results, changed through pipeline phases */
    cv::Mat grayscale_img_;  /**< grayscaled and cropped input image used for efficient algorithm processing  */
    cv::Mat downscaled_img_;  /**< downscaled image used for fast face detection */
    float drowsiness_score_;  /**< driver drowsiness score which indicates level of drivers attention on the road */
    cv::CascadeClassifier face_cascade_;  /**< classifier used for face detection */
    cv::CascadeClassifier eyes_cascade_;  /**< classifier used for eyes detection  */
};



#endif
