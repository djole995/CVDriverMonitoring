#include "DriverDrowsinessPipeline.h"
#include "Constants.h"

#include <iostream>

#ifdef DEBUG_VERSION
#define DEBUG_LOG(str) (std::cout << str << std::endl)
#else
#define DEBUG_LOG(str)
#endif


bool DriverMonitoringPipeline::Init(const std::string &face_cascade_name, const std::string& eyes_cascade_name)
{
    if(!face_cascade_.load(face_cascade_name))
    {
        DEBUG_LOG("Failed to load face cascade!");
        return false;
    }

    if(!eyes_cascade_.load(eyes_cascade_name))
    {
        DEBUG_LOG("Failed to load face cascade!");
        return false;
    }

    data_ = DriverMonitoringData();
    drowsiness_score_ = 0.0f;

    return true;
}

void DriverMonitoringPipeline::Run()
{
    while(true)
    {
        if(!GetFrame())
        {
            DEBUG_LOG("Failed to get frame, terminating program")
            break;
        }

        GrayscaleCropImage();

        DownscaleImage();

        PerformFaceDetection();

        PerformEyesDetection();

        UpdateScore();

        RenderGraphics();
    }
}

void DriverMonitoringPipeline::Shutdown()
{

}

bool DriverMonitoringPipeline::GetFrame()
{
    static cv::VideoCapture capture;
    static bool camera_opened = false;
    std::string fileName("/home/djokicm/RT-RK/vm-shared-dir/frames/DD.mp4");

    if(camera_opened == false)
    {
        if(!capture.open(fileName))
        {
            DEBUG_LOG("Failed to open video!");
            return false;
        }
        else
        {
            camera_opened = true;
        }
    }

    if(!capture.read(data_.original_img_))
    {
        capture.release();
        return false;
    }

    return true;
}

void DriverMonitoringPipeline::GrayscaleCropImage()
{
    grayscale_img_ = data_.original_img_.colRange(HORIZONTAL_CROP, INPUT_RESOLUTION_WIDTH - HORIZONTAL_CROP);
    cv::cvtColor(grayscale_img_, grayscale_img_, CV_BGR2GRAY);
    cv::equalizeHist(grayscale_img_, grayscale_img_);
}

void DriverMonitoringPipeline::DownscaleImage()
{
    cv::resize(grayscale_img_, downscaled_img_, cv::Size(TARGET_RESOLUTION_WIDTH, TARGET_RESOLUTION_HEIGHT));
}

void DriverMonitoringPipeline::PerformFaceDetection()
{
    std::vector<cv::Rect> faces;

    face_cascade_.detectMultiScale(downscaled_img_, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, cv::Size(20, 20));

    if(faces.size() > 0)
    {
        float ratioX = (float)(INPUT_RESOLUTION_WIDTH - 2*HORIZONTAL_CROP)/
                       TARGET_RESOLUTION_WIDTH;
        float ratioY = (float)(INPUT_RESOLUTION_HEIGHT - 2*VERTICAL_CROP)/
                       TARGET_RESOLUTION_HEIGHT;

        faces[0].x = faces[0].x * ratioX;
        faces[0].y = faces[0].y * ratioY;
        faces[0].width *= ratioX;
        faces[0].height *= ratioY;

        data_.face_found_ = true;
        data_.face_region_ = faces[0];
    }
}

void DriverMonitoringPipeline::PerformEyesDetection()
{
    std::vector<cv::Rect> left_eye;
    std::vector<cv::Rect> right_eye;

    cv::Mat left_eye_ROI;
    cv::Mat right_eye_ROI;

    // if drivers face is not found, there is no need to search for eyes
    if(!data_.face_found_)
    {
        data_.eyes_found_ = false;
        return;
    }

    data_.left_eye_region_ = cv::Rect(data_.face_region_.x + data_.face_region_.width / 8,
                           data_.face_region_.y + data_.face_region_.height * 0.31,
                           data_.face_region_.width * 0.3,
                           data_.face_region_.height * 0.2);

    data_.right_eye_region_ = cv::Rect(data_.left_eye_region_.x + data_.left_eye_region_.width + data_.face_region_.width / 8,
                            data_.face_region_.y + data_.face_region_.height * 0.31,
                            data_.face_region_.width * 0.3,
                            data_.face_region_.height * 0.2);


    left_eye_ROI = grayscale_img_(data_.left_eye_region_);
    right_eye_ROI = grayscale_img_(data_.right_eye_region_);

    //-- In each face, detect eyes
    eyes_cascade_.detectMultiScale(left_eye_ROI, left_eye, 1.1, 4, 0 | CV_HAAR_SCALE_IMAGE,
                                   cv::Size(data_.face_region_.width * 0, data_.face_region_.height * 0), cv::Size(data_.face_region_.width * 0.35, data_.face_region_.height * 0.30));

    eyes_cascade_.detectMultiScale(right_eye_ROI, right_eye, 1.1, 4, 0 | CV_HAAR_SCALE_IMAGE,
        cv::Size(data_.face_region_.width * 0, data_.face_region_.height * 0), cv::Size(data_.face_region_.width * 0.35, data_.face_region_.height * 0.30));

    // DEBUG : draw rectangles over open eyes.
/*    for(size_t i = 0; i < 1; i++)
    {
        cv::rectangle(data_.original_img_, cv::Rect(eyes[i].x + data_.right_eye_region_.x + HORIZONTAL_CROP, eyes[i].y + data_.right_eye_region_.y, eyes[i].width, eyes[i].height), cv::Scalar(0, 255, 0));
    }*/

    if(left_eye.size() == 0 && right_eye.size() == 0)
    {
        data_.eyes_found_ = false;
    }
    else
    {
        data_.eyes_found_ = true;
    }

    data_.face_region_.x += HORIZONTAL_CROP;
    data_.face_region_.y += VERTICAL_CROP;

    data_.left_eye_region_.x += HORIZONTAL_CROP;
    data_.left_eye_region_.y += VERTICAL_CROP;

    data_.right_eye_region_.x += HORIZONTAL_CROP;
    data_.right_eye_region_.y += VERTICAL_CROP;

}

void DriverMonitoringPipeline::UpdateScore()
{
    //Case when drowsiness score is at low level
    if(drowsiness_score_ < driver_drowsiness_Low_Limit)
    {
        if(data_.eyes_found_ && data_.face_found_)
        {
            drowsiness_score_ -= driver_drowsiness_Medium_Price/driver_drowsiness_FPS;
            if(drowsiness_score_ < driver_drowsiness_Min_Score)
            {
                drowsiness_score_ = driver_drowsiness_Min_Score;
            }
        }
        else
        {
            if(!data_.face_found_)
            {
                drowsiness_score_ += driver_drowsiness_Low_Price/driver_drowsiness_FPS;
            }
            else
            {
                drowsiness_score_ += driver_drowsiness_Medium_Price/driver_drowsiness_FPS;
            }
        }
    }
    //Case when drowsiness score is at medium level
    else if(drowsiness_score_ < driver_drowsiness_Medium_Limit)
    {
        if(data_.eyes_found_ && data_.face_found_)
        {
            drowsiness_score_ -= driver_drowsiness_Low_Price/driver_drowsiness_FPS;
        }
        else
        {
            if(!data_.face_found_)
            {
                drowsiness_score_ += driver_drowsiness_Medium_Price/driver_drowsiness_FPS;
            }
            else
            {
                drowsiness_score_ += driver_drowsiness_High_Price/driver_drowsiness_FPS;
            }
        }
    }
    else //Case when drowsiness score is at high level
    {
        if(data_.eyes_found_ && data_.face_found_)
        {
            drowsiness_score_ -= driver_drowsiness_High_Price/driver_drowsiness_FPS;
        }
        else
        {
            drowsiness_score_ += driver_drowsiness_High_Price/driver_drowsiness_FPS;
            if(drowsiness_score_ > driver_drowsiness_Max_Score)
            {
                drowsiness_score_ = driver_drowsiness_Max_Score;
            }
        }
    }

}

void DriverMonitoringPipeline::RenderGraphics()
{
    std::string name = "window";
    cv::Scalar eyes_rect_color;
    cv::Rect scorebar(10, data_.original_img_.rows - drowsiness_score_ * 20.f, 50, data_.original_img_.rows);

    if(data_.face_found_)
    {
        if(data_.eyes_found_)
        {
            eyes_rect_color = cv::Scalar(0, 255, 0);
        }
        else
        {
            eyes_rect_color = cv::Scalar(0, 0, 255);
        }


        cv::rectangle(data_.original_img_, data_.face_region_, cv::Scalar(0, 0, 255));
        cv::rectangle(data_.original_img_, data_.left_eye_region_, eyes_rect_color);
        cv::rectangle(data_.original_img_, data_.right_eye_region_, eyes_rect_color);
    }

    cv::rectangle(data_.original_img_, scorebar, eyes_rect_color, 50, cv::LINE_AA);

    cv::imshow(name, data_.original_img_);
    cv::waitKey(1);
}
