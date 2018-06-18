#include <opencv2/objdetect.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv/cv.h>

#include <iostream>
#include <stdio.h>


// get frame
// crop
// face detection
// calculate eyes region
// eyes detection
// update score

std::string fileName("/home/djokicm/RT-RK/vm-shared-dir/frames/eyes_detection_avi.avi");

/** Function Headers */
void detectAndDisplay( cv::Mat frame);

/** Global variables */
std::string face_cascade_name = "../haarcascade_frontalface_alt.xml";
std::string eyes_cascade_name = "../haarcascade_righteye_2splits.xml";
cv::CascadeClassifier face_cascade;
cv::CascadeClassifier eyes_cascade;
std::string window_name = "Capture - Face detection";

int main( int argc, const char** argv )
{
    cv::VideoCapture capture;
    cv::Mat frame;
    cv::Mat croppedFrame;

    //-- 1. Load the cascades
    if( !face_cascade.load( face_cascade_name ) ){ printf("--(!)Error loading\n"); return -1; };
    if( !eyes_cascade.load( eyes_cascade_name ) ){ printf("--(!)Error loading\n"); return -1; };

    //-- 2. Read the video stream

    if(!capture.open(fileName))
    {
        std::cout << "Failed to open video!" << std::endl;
        return -1;
    }


    while( true )
    {
        capture.read(frame);

        //-- 3. Apply the classifier to the frame
        if( !frame.empty() )
        {
            croppedFrame = frame.colRange(206, 1074);

            detectAndDisplay( croppedFrame );
        }
        else
        {
            printf(" --(!) No captured frame -- Break!"); break;
        }

        int c = cv::waitKey(1);
        if( (char)c == 'c' ) { break; }
    }

    return 0;
}

void detectAndDisplay( cv::Mat frame )
{
    std::vector<cv::Rect> faces;
    cv::Mat frame_gray;

    cv::cvtColor( frame, frame_gray, CV_BGR2GRAY);
    cv::equalizeHist( frame_gray, frame_gray );

    //-- Detect faces
    face_cascade.detectMultiScale(frame_gray, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, cv::Size(100, 100));

    for(size_t i = 0; i < faces.size(); i++)
    {
        cv::rectangle(frame, faces[i], cv::Scalar(0, 0, 255));

        std::vector<cv::Rect> eyes;
        cv::Rect eyesSerchArea(faces[i].x + faces[i].width / 10, faces[i].y + faces[i].height * 0.20, faces[i].width * 0.8, faces[i].height * 0.3);

        cv::Mat eyesROI = frame_gray(eyesSerchArea);
        //-- In each face, detect eyes
        eyes_cascade.detectMultiScale( eyesROI, eyes, 1.1, 3, 0 |CV_HAAR_SCALE_IMAGE, cv::Size(faces[i].width * 0.35 * 0, faces[i].height * 0.30 * 0), cv::Size(faces[i].width * 0.35, faces[i].height * 0.30));
        //eyes_cascade.detectSingleScale();
        rectangle(frame, eyesSerchArea, cv::Scalar( 255, 0, 0 ));


        for(size_t j = 0; j < eyes.size(); j++)
        {
            rectangle(frame, cv::Rect(eyes[i].x + eyesSerchArea.x, eyes[i].y + eyesSerchArea.y, eyes[i].width, eyes[i].height), cv::Scalar(0, 255, 0));
        }
    }
    //-- Show what you got
    cv::imshow(window_name, frame);
}

