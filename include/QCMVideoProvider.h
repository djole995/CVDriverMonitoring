#ifndef _QCM_VIDEO_PROVIDER_H_
#define _QCM_VIDEO_PROVIDER_H_

#include "VideoProvider.h"
#include "../common/CameraInput/inc/CameraInput.h"
#include <mutex>
#include <memory>

#define QCM_CAM_FRAME_WIDTH 1280
#define QCM_CAM_FRAME_HEIGHT 720

constexpr size_t QCM_CAM_FRAME_SIZE = 2 * QCM_CAM_FRAME_WIDTH * QCM_CAM_FRAME_HEIGHT;

/**
 * @brief QCM camera stream provider
 */
class QCMVideoProvider : public VideoProvider
{

public:

    /**
     * @brief constructor
     */
    QCMVideoProvider() = default;

    /**
     * @brief destructor
     */
    ~QCMVideoProvider() = default;

    /**
     * @brief init camera provider
     * @param camera_id camera from which stream shall be acquired
     * @return true if camera opening is successfull, false otherwise
     */
    bool Init(CAM_Cameras camera_id);

    /**
     * @brief Acquire camera frame
     * @param frame [out] acquired frame
     * @return true on success, false if error occurs
     */
    bool GetFrame(cv::Mat &frame) override;

    /**
     * @brief Cleanup camera provider resources
     *
     * @return true on success, false otherwise
     */
    bool Shutdown();

    /**
     * @brief Camera callback which will be triggered every time when new frame is available
     * @param new_frame new camera frame
     * @param stream_id stream from which frame arrived
     */
    friend void FrameArrived(uint8_t* new_frame, int stream_id);

private:

    uint8_t frame_[QCM_CAM_FRAME_SIZE]; /**< acquired camera frame */

    std::mutex frame_lock;  /**< lock used for synchronization of acquiring camera frame and providing it to users when GetFrame is called. */

    CAM_Cameras camera_id_; /**< used camera id */

    std::unique_ptr<cameraFrameCb> camera_callback;

};

#endif
