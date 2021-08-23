#ifndef SCLITEGRABBER_H
#define SCLITEGRABBER_H

#include "basegrabber.h"

#include <mutex>
#include <thread>
#include <atomic>

#include "ScreenCapture.h"

class Handle
{
public:
    Handle();
    Handle(int mon_index, int fps);
    Handle(const Handle&) = delete;
    void update();
    void swap(cv::Mat mat);
    void setGrabberCallback(GrabberCallback callback);
    friend class SCLiteGrabber;
private:
    cv::Mat mat_;
    std::mutex swap_mutex_;
    std::thread send_thr_;
    std::atomic<bool> is_stop_send_;
    std::chrono::high_resolution_clock::time_point last_written_;
    int fps_;
    int monitor_index_;
    GrabberCallback callback_;
};

class SCLiteGrabber: public BaseGrabber
{
public:
    virtual void init(GrabberCallback callback, int grabbing_fps) override;
    virtual std::vector<MonitorDesc> monitors() override;
    ~SCLiteGrabber();
private:
    std::vector<std::shared_ptr<Handle>> monitors_;
    std::shared_ptr<SL::Screen_Capture::IScreenCaptureManager> grabber_;
};

#endif // SCLITEGRABBER_H
