#include "DriverDrowsinessPipeline.h"
#include "Constants.h"
#include "VideoProvider.h"
#include "DebugUtils.h"

extern bool app_terminated;

bool DriverMonitoringPipeline::Init(const std::string& face_cascade_name, const std::string& eyes_cascade_name)
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

void DriverMonitoringPipeline::Run(VideoProvider& video_provider)
{
    while(!app_terminated)
    {
        if(!video_provider.GetFrame(data_.original_img_))
        {
            DEBUG_LOG("Failed to get frame, breaking algorithm loop!");
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

float DriverMonitoringPipeline::GetDrowsinessScore()
{
    return drowsiness_score_;
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
        int best_index = 0;

        // If multiple faces are found, select as drivers the one which is closest to image center
        for(int i = 1; i < faces.size(); i++)
        {
            int downscaled_img_center = TARGET_RESOLUTION_WIDTH >> 1;
            int best_center_distance = abs(faces[best_index].x - downscaled_img_center);
            if(abs(faces[i].x - downscaled_img_center) < best_center_distance)
            {
                best_index = i;
            }
        }

        // upscale coordinates to cropped resolution
        faces[best_index].x = faces[best_index].x * ratioX;
        faces[best_index].y = faces[best_index].y * ratioY;
        faces[best_index].width *= ratioX;
        faces[best_index].height *= ratioY;

        data_.face_found_ = true;
        data_.face_region_ = faces[best_index];
    }
    else
    {
        data_.face_found_ = false;
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
    cv::Scalar scorebar_color;
    cv::Rect scorebar(10, data_.original_img_.rows - drowsiness_score_ * 50.f, 30, data_.original_img_.rows);

    if(data_.face_found_)
    {
        if(data_.eyes_found_)
        {
            eyes_rect_color = cv::Scalar(0, 255, 0);
            scorebar_color = cv::Scalar(0, 255, 0);
        }
        else
        {
            eyes_rect_color = cv::Scalar(0, 0, 255);
            scorebar_color = cv::Scalar(0, 0, 255);
        }


        cv::rectangle(data_.original_img_, data_.face_region_, cv::Scalar(0, 0, 255));
        cv::rectangle(data_.original_img_, data_.left_eye_region_, eyes_rect_color);
        cv::rectangle(data_.original_img_, data_.right_eye_region_, eyes_rect_color);
    }
    else
    {
       scorebar_color = cv::Scalar(0, 0, 255);
    }

    cv::rectangle(data_.original_img_, scorebar, scorebar_color, -1);

    cv::imshow(name, data_.original_img_);
    cv::waitKey(1);
}
