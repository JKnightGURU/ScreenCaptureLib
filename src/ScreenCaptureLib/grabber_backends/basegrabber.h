#ifndef BASEGRABBER_H
#define BASEGRABBER_H

#include <opencv2/core.hpp>
#include <functional>

typedef std::function<void(cv::Mat, int mon_index)> GrabberCallback;

struct MonitorDesc
{
    int index_;
    int width_;
    int height_;
    int offset_x_;
    int offset_y_;
};

class BaseGrabber
{
public:
    virtual void init(GrabberCallback callback, int grabbing_fps) = 0;
    virtual std::vector<MonitorDesc> monitors() = 0;
    virtual ~BaseGrabber() { }

};

#endif // BASEGRABBER_H
