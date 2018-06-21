#include <opencv2/objdetect.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv/cv.h>

#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

#ifdef USE_ARA_API
#include <exec/application_state_client.hpp>
#endif

#include "DriverDrowsinessPipeline.h"
#include "CVVideoProvider.h"
#include "DebugUtils.h"

bool app_terminated = false;

static void terminate(int signum)
{
    DEBUG_LOG("SIGTERM received, terminating application!");
    app_terminated = true;
}

int main()
{
    std::string face_cascade_name = "../cascades/haarcascade_frontalface_alt.xml";
    std::string eyes_cascade_name = "../cascades/haarcascade_eye.xml";
    std::string fileName("/home/djokicm/RT-RK/vm-shared-dir/frames/DD.mp4");

    DriverMonitoringPipeline pipeline;
    CVVideoProvider cv_video_provider;

    //Set SIGTERM handler
    struct sigaction action;
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = terminate;
    sigaction(SIGTERM, &action, NULL);

    // integrate with ARA Execution Manager
#ifdef USE_ARA_API
    ara::exec::applicationstate::ApplicationStateClient app_state_client;

    // report to EM that app is running
    app_state_client.ReportApplicationState(ara::exec::applicationstate::ApplicationState::kRunning);
#endif

    cv_video_provider.Init(fileName);

    // init algorithm pipeline and load classifiers
    pipeline.Init(face_cascade_name, eyes_cascade_name);

    // run pipeline loop
    pipeline.Run(cv_video_provider);

    // cleanup resources when sigterm is received
    pipeline.Shutdown();
    cv_video_provider.Shutdown();

    return 0;
}
