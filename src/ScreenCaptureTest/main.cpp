#include "screenrecorder.h"

// #include "basevideowriter.h"

#include <thread>
#include <iostream>

#include <opencv2/imgproc.hpp>

int main()
{
    sc_api::ScreenRecorder recorder;

    recorder.init(5, 10);

    // std::this_thread::sleep_for(std::chrono::seconds(10));

    recorder.startRecording(".", "out_mon_t1_", sc_api::wbFFMPEG);

    double record_min   = 1 / 6.;
    int    interval_sec = 5;
    int    current_sec  = 0;
    int    max_sec      = record_min * 60;

    while (current_sec < max_sec)
    {
        std::this_thread::sleep_for(std::chrono::seconds(interval_sec));
        current_sec += interval_sec;
        std::cout << "RECORDING FOR " << current_sec / 60 << " MINUTES, " << current_sec % 60 << " SECONDS" << std::endl;
    }
    recorder.stopRecording();


    /*
        auto writer = sc_api::createVideoWriter(sc_api::wbFFMPEG, { "out.mp4", 25, 1024, 768, 800000 });

        for (int i = 0; i < 500; i++)
        {
            cv::Mat   mat = cv::Mat::zeros(768, 1024, CV_8UC3);
            cv::Point pt1(rand() % 1024, rand() % 768);
            cv::Point pt2(rand() % 1024, rand() % 768);
            cv::rectangle(mat, pt1, pt2, cv::Scalar::all(255), 8);

            writer->write(mat);
        }*/
}
