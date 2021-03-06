#include <opencv2/objdetect.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

#ifdef USE_ARA_API
#include <exec/application_state_client.hpp>
#include "DDDiagService.h"

#include <thread>
#endif

#include "DriverDrowsinessPipeline.h"
#include "DebugUtils.h"

#ifdef TARGET_QCM
#include "qcm/QCMVideoProvider.h"
#else
#include "CVVideoProvider.h"
#endif

bool app_terminated = false;

static void terminate(int signum)
{
    DEBUG_LOG("SIGTERM received, terminating application!");
    app_terminated = true;
}

int main()
{
#ifdef USE_ARA_API
    std::string face_cascade_name = "/opt/CVDriverMonitoring/cascades/haarcascade_frontalface_alt.xml";
    std::string eyes_cascade_name = "/opt/CVDriverMonitoring/cascades/haarcascade_eye.xml";
#else
    std::string face_cascade_name = "../cascades/haarcascade_frontalface_alt.xml";
    std::string eyes_cascade_name = "../cascades/haarcascade_eye.xml";
#endif

#ifdef TARGET_QCM
    CAM_Cameras video_provider_param = CAM_4;
#else
    std::string video_provider_param("0");
#endif

    DriverMonitoringPipeline pipeline;

#ifdef TARGET_QCM
    QCMVideoProvider video_provider;
#else
    CVVideoProvider video_provider;
#endif

    //Set SIGTERM handler
    struct sigaction action;
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = terminate;
    sigaction(SIGTERM, &action, NULL);
    sigaction(SIGKILL, &action, NULL);

    // integrate with ARA Execution Manager
#ifdef USE_ARA_API
    ara::exec::applicationstate::ApplicationStateClient app_state_client;
    DDDiagService diag_service;

    // report to EM that app is running
    app_state_client.ReportApplicationState(ara::exec::applicationstate::ApplicationState::kRunning);

    diag_service.init();
#endif

    if(!video_provider.Init(video_provider_param))
    {
        DEBUG_LOG("Failed to init video source");
        return -1;
    }

    // init algorithm pipeline and load classifiers
    pipeline.Init(face_cascade_name, eyes_cascade_name);

#ifdef USE_ARA_API
    std::thread
    ([&]()
        {
            while(!app_terminated)
            {
                uint8_t score = static_cast<uint8_t>(pipeline.GetDrowsinessScore());

                diag_service.updateState(score);

                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }
    ).detach();
#endif

    // run pipeline loop
    pipeline.Run(video_provider);

    // cleanup resources when sigterm is received
#ifdef USE_ARA_API
    diag_service.Shutdown();
#endif
    pipeline.Shutdown();
    video_provider.Shutdown();

    return 0;
}
