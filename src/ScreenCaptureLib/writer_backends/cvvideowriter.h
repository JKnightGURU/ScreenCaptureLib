#pragma once

#include <memory>

#include <opencv2/videoio.hpp>

#include "ScreenCaptureLib/basevideowriter.hpp"

namespace sc_api {

class CVVideoWriter : public BaseVideoWriter
{
public:
    virtual void write(cv::Mat mat) override;

protected:
    virtual void initImpl() override;

private:
    std::shared_ptr<cv::VideoWriter> writer_;
};

}
