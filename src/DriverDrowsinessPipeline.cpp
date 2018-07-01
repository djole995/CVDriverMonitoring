#include "DriverDrowsinessPipeline.h"
#include "Constants.h"
#include "VideoProvider.h"
#include "DebugUtils.h"
#include "RenderGraphics.h"

#include <thread>
#include <chrono>

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

    initRenderTarget(1280, 720);

    return true;
}

void DriverMonitoringPipeline::Run(VideoProvider& video_provider)
{
    static double avg_fps = 0;
    static int cnt = 0;
    while(!app_terminated)
    {
        auto start = std::chrono::high_resolution_clock::now();

        if(!video_provider.GetFrame(data_.original_img_))
        {
            DEBUG_LOG("Failed to get frame, breaking algorithm loop!");
            break;
        }

        GrayscaleCropImage();
        DEBUG_LOG("Grayscale crop image complete.");

        DownscaleImage();
        DEBUG_LOG("Downscale image complete.");

        PerformFaceDetection();
        DEBUG_LOG("Face detection complete.");

        PerformEyesDetection();
        DEBUG_LOG("Eyes detection complete.");

        UpdateScore();
        DEBUG_LOG("Score update complete.");

        RenderGraphics();
        DEBUG_LOG("Grpahic rendering complete.");

        auto finish = std::chrono::high_resolution_clock::now();

        std::chrono::duration<double> elapsed_time = finish - start;

        avg_fps += elapsed_time.count();
        cnt++;

        if(cnt == 2000)
        {
            avg_fps /= cnt;
            std::cout << "avg fps: " << avg_fps << std::endl;
        }
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
    cv::cvtColor(grayscale_img_, grayscale_img_, CV_YUV2GRAY_UYVY);
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
    eyes_cascade_.detectMultiScale(left_eye_ROI, left_eye, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE,
                                   cv::Size(data_.face_region_.width * 0, data_.face_region_.height * 0), cv::Size(data_.face_region_.width * 0.35, data_.face_region_.height * 0.30));

    eyes_cascade_.detectMultiScale(right_eye_ROI, right_eye, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE,
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
    static Rectangle r[80];
    int rect_cnt = 0;
    // Drowsiness score bar
    static V_XYZ score_bar_rect = { 10, 700, 0 };
    static int score_rect_width = 20;
    static int score_rect_height = 10;
    static int rects_offset = 0;
    // Render algorithm output on screen.

    // If frontal face is found set its rectangle
    if(data_.face_found_)
    {
        // Face region
        r[0].vertices[0].x = data_.face_region_.x;
        r[0].vertices[0].y = data_.face_region_.y;
        r[0].vertices[0].z = 0;

        r[0].vertices[1].x = data_.face_region_.x + data_.face_region_.width;
        r[0].vertices[1].y = data_.face_region_.y;
        r[0].vertices[1].z = 0;

        r[0].vertices[2].x = data_.face_region_.x + data_.face_region_.width;
        r[0].vertices[2].y = data_.face_region_.y + data_.face_region_.height;
        r[0].vertices[2].z = 0;

        r[0].vertices[3].x = data_.face_region_.x;
        r[0].vertices[3].y = data_.face_region_.y + data_.face_region_.height;
        r[0].vertices[3].z = 0;

        r[0].r = 1.0f;
        r[0].g = 0.0f;
        r[0].b = 0.0f;

        r[0].color_fill = false;

        rect_cnt++;

        // if eyes are detected and opened, set eyes region rectangles
        // Left eye region
        r[1].vertices[0].x = data_.left_eye_region_.x;
        r[1].vertices[0].y = data_.left_eye_region_.y;
        r[1].vertices[0].z = 0;

        r[1].vertices[1].x = data_.left_eye_region_.x + data_.left_eye_region_.width;
        r[1].vertices[1].y = data_.left_eye_region_.y;
        r[1].vertices[1].z = 0;

        r[1].vertices[2].x = data_.left_eye_region_.x + data_.left_eye_region_.width;
        r[1].vertices[2].y = data_.left_eye_region_.y + data_.left_eye_region_.height;
        r[1].vertices[2].z = 0;

        r[1].vertices[3].x = data_.left_eye_region_.x;
        r[1].vertices[3].y = data_.left_eye_region_.y + data_.left_eye_region_.height;
        r[1].vertices[3].z = 0;

        r[1].color_fill = false;

        // Right eye region
        r[2].vertices[0].x = data_.right_eye_region_.x;
        r[2].vertices[0].y = data_.right_eye_region_.y;
        r[2].vertices[0].z = 0;

        r[2].vertices[1].x = data_.right_eye_region_.x + data_.right_eye_region_.width;
        r[2].vertices[1].y = data_.right_eye_region_.y;
        r[2].vertices[1].z = 0;

        r[2].vertices[2].x = data_.right_eye_region_.x + data_.right_eye_region_.width;
        r[2].vertices[2].y = data_.right_eye_region_.y + data_.right_eye_region_.height;
        r[2].vertices[2].z = 0;

        r[2].vertices[3].x = data_.right_eye_region_.x;
        r[2].vertices[3].y = data_.right_eye_region_.y + data_.right_eye_region_.height;
        r[2].vertices[3].z = 0;

        r[2].color_fill = false;

        rect_cnt += 2;

        if(data_.eyes_found_)
        {
            r[1].r = 0.0f;
            r[1].g = 1.0f;
            r[1].b = 0.0f;

            r[2].r = 0.0f;
            r[2].g = 1.0f;
            r[2].b = 0.0f;
        }
        else
        {
            r[1].r = 1.0f;
            r[1].g = 0.0f;
            r[1].b = 0.0f;

            r[2].r = 1.0f;
            r[2].g = 0.0f;
            r[2].b = 0.0f;
        }
    }

    // calculate height of score bar on the base of current drowsiness score
    for(int k = 0; k < 60; k++)
    {
        r[rect_cnt].vertices[0].x = score_bar_rect.x;
        r[rect_cnt].vertices[0].y = score_bar_rect.y - k * (score_rect_height + rects_offset);
        r[rect_cnt].vertices[1].x = score_bar_rect.x + score_rect_width;
        r[rect_cnt].vertices[1].y = score_bar_rect.y - k * (score_rect_height + rects_offset);
        r[rect_cnt].vertices[2].x = score_bar_rect.x + score_rect_width;
        r[rect_cnt].vertices[2].y = score_bar_rect.y + (1-k) * score_rect_height - k * rects_offset;
        r[rect_cnt].vertices[3].x = score_bar_rect.x;
        r[rect_cnt].vertices[3].y = score_bar_rect.y + (1-k) * score_rect_height - k * rects_offset;

        r[rect_cnt].r = k / 60.0f;
        r[rect_cnt].g = (60-k) / 60.0f;
        r[rect_cnt].b = 0.0f;

        r[rect_cnt].color_fill = (drowsiness_score_ > k / 6.0f) ? true : false;

        if(r[rect_cnt].color_fill)
        {
            rect_cnt++;
        }
        else
        {
            break;
        }
    }

    render(r, rect_cnt, data_.original_img_.data);
}
