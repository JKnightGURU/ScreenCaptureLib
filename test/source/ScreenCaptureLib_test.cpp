#include <ScreenCaptureLib/ScreenCaptureLib.hpp>

#include <thread>
#include <iostream>

#include <opencv2/imgproc.hpp>

void verbose_sleep (unsigned seconds)
{
    const char symbs[] = "|/-\\|/-\\";

    const auto start_point = std::chrono::steady_clock::now();

    auto counter = 0U;
    while (true) {
        const auto diff_seconds = std::chrono::duration_cast<std::chrono::seconds> (std::chrono::steady_clock::now() - start_point).count();

        if (diff_seconds >= seconds)
        {
            break;
        }

        std::cout << "\r"
                  << '[' << symbs[counter++ % (sizeof symbs - 1)] << ']'
                  << " wait another "
                  << seconds - diff_seconds
                  << " seconds " << std::flush;

        std::this_thread::sleep_for (std::chrono::milliseconds (200));
    }

    std::cout << '\r' << std::flush;
}

void recorder_test()
{
    sc_api::ScreenRecorder recorder;

    // (buffer_size_in_seconds, desired_FPS)
    recorder.init(5, 10);

    // Let recorder thread work for 10 seconds
    verbose_sleep (10);

    // Initialize recording with the following parameters:
    // Recording directory: "."
    // Prefix (will be concatenated with a screen number): "out_mon_t1_"
    // API type (simply use wbFFMPEG)
    recorder.startRecording(".", "out_mon_t1_", sc_api::wbFFMPEG);

    // Sleep for 20 seconds more
    verbose_sleep(20);

    // In the end, get a file named "out_mon_t1_0.mp4"
    // in a desired directory, with the length of 25 seconds.
    // 5: buffered, from the first sleep
    // 20: during the last sleep
    recorder.stopRecording();
}

void videowriter_test()
{
    // initialize writer: using FFMPEG, { filename, FPS, width, height, bitrate }

    auto writer = sc_api::createVideoWriter(sc_api::wbFFMPEG, sc_api::VWSettings { "out.mp4", 25, 1024, 768, 800000 });

    // let's generate some frames

    for (int i = 0; i < 500; i++)
    {
        cv::Mat   mat = cv::Mat::zeros(768, 1024, CV_8UC3);
        cv::Point pt1(rand() % 1024, rand() % 768);
        cv::Point pt2(rand() % 1024, rand() % 768);
        cv::rectangle(mat, pt1, pt2, cv::Scalar::all(255), 8);

        // and simply write them into a writer object
        writer->write(mat);
    }
}

auto main() -> int
{
    recorder_test();
    videowriter_test();

    return EXIT_SUCCESS;
}
