#pragma once

#include "ScreenCaptureLib/basevideowriter.hpp"

extern "C" {
#include <libswscale/swscale.h>
#include <libavcodec/avcodec.h>
#include <libavutil/mathematics.h>
#include <libavformat/avformat.h>
#include <libavutil/opt.h>
}

namespace sc_api {

class FFmpegVideoWriter : public BaseVideoWriter
{
public:
    virtual void write(cv::Mat mat) override;
    ~FFmpegVideoWriter();

protected:
    virtual void initImpl() override;

private:
    unsigned int iframe;

    SwsContext* swsCtx;
    AVOutputFormat* fmt;
    AVStream* stream;
    AVFormatContext* fc;
    AVCodecContext* c;
    AVPacket pkt;

    AVFrame* rgbpic, * yuvpic;
};

}
