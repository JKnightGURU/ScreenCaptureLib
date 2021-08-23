#include "sclitegrabber.h"

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "Dwmapi.lib")

void ExtractAndConvertToRGB(const SL::Screen_Capture::Image &img, unsigned char *dst)
{
    auto imgsrc = StartSrc(img);
    auto imgdist = dst;
    for (auto h = 0; h < Height(img); h++) {
        auto startimgsrc = imgsrc;
        for (auto w = 0; w < Width(img); w++) {
            *imgdist++ = imgsrc->B;
            *imgdist++ = imgsrc->G;
            *imgdist++ = imgsrc->R;
            imgsrc++;
        }
        imgsrc = SL::Screen_Capture::GotoNextRow(img, startimgsrc);
    }
}

void SCLiteGrabber::init(GrabberCallback callback, int grabbing_fps)
{
    auto monitors = SL::Screen_Capture::GetMonitors();

    monitors_.clear();
    for(int i = 0; i < monitors.size(); i++)
    {
        monitors_.push_back(std::make_shared<Handle>(i, grabbing_fps));
        monitors_.back()->setGrabberCallback(callback);
    }

    grabber_ = SL::Screen_Capture::CreateCaptureConfiguration(
                static_cast<std::function<std::vector<SL::Screen_Capture::Monitor>()>>(SL::Screen_Capture::GetMonitors))
    ->onNewFrame([&](const SL::Screen_Capture::Image& img, const SL::Screen_Capture::Monitor& monitor)
    {
        cv::Mat mat = cv::Mat::zeros(cv::Size(monitor.Width, monitor.Height), CV_8UC3);
        ExtractAndConvertToRGB(img, mat.data);
        monitors_[monitor.Index]->swap(mat);
    })->start_capturing();

    grabber_->setFrameChangeInterval(std::chrono::milliseconds(1000/grabbing_fps));

    for(int i = 0; i < monitors_.size(); i++)
    {
        monitors_[i]->send_thr_ = std::thread([i, this](){
            while (!monitors_[i]->is_stop_send_)
                if (monitors_[i]->mat_.data)
                    monitors_[i]->update();
        });
    }
}

std::vector<MonitorDesc> SCLiteGrabber::monitors()
{
    auto monitors = SL::Screen_Capture::GetMonitors();
    std::vector<MonitorDesc> descs;

    for(int i = 0; i < monitors.size(); i++)
        descs.push_back({ i, monitors[i].Width, monitors[i].Height, -1, -1});

    return descs;
}

SCLiteGrabber::~SCLiteGrabber()
{
    for(int i = 0; i < monitors_.size(); i++)
    {
        monitors_[i]->is_stop_send_ = true;
        monitors_[i]->send_thr_.join();
    }
}

Handle::Handle():
    callback_([](cv::Mat, int){}),
    is_stop_send_(false)
{ }

Handle::Handle(int mon_index, int fps):
    monitor_index_(mon_index),
    fps_(fps),
    callback_([](cv::Mat, int){}),
    is_stop_send_(false)
{ }

void Handle::update()
{
    std::chrono::high_resolution_clock::time_point current =
            std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> time_span =
            std::chrono::duration_cast<std::chrono::duration<double>>(current - last_written_);

    int msec_span = time_span.count() * 1000;
    int for_frame = 1000 / fps_;

    if (msec_span < for_frame)
        std::this_thread::sleep_for(std::chrono::milliseconds(for_frame - msec_span));

    last_written_ = std::chrono::high_resolution_clock::now();

    std::lock_guard<std::mutex> lock(swap_mutex_);
    callback_(mat_, monitor_index_);
}

void Handle::swap(cv::Mat mat)
{
    std::lock_guard<std::mutex> lock(swap_mutex_);
    mat_ = mat;
}

void Handle::setGrabberCallback(GrabberCallback callback)
{
    this->callback_ = callback;
}
