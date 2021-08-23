#include "cvvideowriter.h"

namespace sc_api {

void CVVideoWriter::write(cv::Mat mat)
{
    if (writer_)
        writer_->write(mat);
}

void CVVideoWriter::initImpl()
{
    writer_ = std::make_shared<cv::VideoWriter>(
        settings_.fname_,
        cv::VideoWriter::fourcc('X', '2', '6', '4'),
        settings_.fps_,
        cv::Size(settings_.width_, settings_.height_));
}

}
