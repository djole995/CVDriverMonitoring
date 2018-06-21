#include <opencv2/objdetect.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv/cv.h>

#include <iostream>
#include <stdio.h>
#include <unistd.h>

#include "DriverDrowsinessPipeline.h"

/** Global variables */
std::string face_cascade_name = "../haarcascade_frontalface_alt.xml";
std::string eyes_cascade_name = "../haarcascade_eye.xml";

int main(int argc, const char** argv)
{
    DriverMonitoringPipeline pipeline;

    pipeline.Init(face_cascade_name, eyes_cascade_name);

    pipeline.Run();

    return 0;
}
