#ifndef XSHMGRABBER_H
#define XSHMGRABBER_H

#include "basegrabber.h"
#include <X11/Xlib.h>
#include <X11/extensions/XShm.h>

#include <thread>
#include <atomic>

class XShmScreenshot{
public:
    XShmScreenshot(int x_, int y_, int width_, int height_);
    void operator() (cv::Mat& cv_img);
    ~XShmScreenshot();
    void Init();
private:
    Display* display_;
    Window root_;
    XWindowAttributes window_attributes_;
    Screen* screen_;
    XImage* ximg_;
    XShmSegmentInfo shminfo_;

    int x_, y_, width_, height_;

    bool init_;
};


class XShmGrabber: public BaseGrabber
{
public:
    XShmGrabber();
    ~XShmGrabber();
    void init(GrabberCallback callback, int grabbing_fps);
    std::vector<MonitorDesc> monitors();
private:
    std::vector<std::shared_ptr<XShmScreenshot>> monitors_;

    GrabberCallback callback_;
    std::thread grab_thread_;
    std::shared_ptr<std::atomic<bool>> is_stop_requested_;
};

#endif // XSHMGRABBER_H
