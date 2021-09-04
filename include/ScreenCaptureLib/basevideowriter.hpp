#pragma once

#include <opencv2/core.hpp>
#include <cstdint>
#include <memory>

#include "ScreenCaptureLib/defs/scap_defs.hpp"
#include "ScreenCaptureLib/ScreenCaptureLib_export.h"

namespace sc_api {

struct VWSettings
{
    char fname_[512];
    int  fps_;
    int  width_, height_;
    int  bitrate_ = 800000;
};

class SCREENCAPTURELIB_EXPORT BaseVideoWriter
{
public:
    BaseVideoWriter() = default;
    void init(VWSettings sets);

    virtual void write(cv::Mat mat) = 0;
    virtual ~BaseVideoWriter() {}

    BaseVideoWriter& operator=(const BaseVideoWriter&) = delete;
    BaseVideoWriter& operator=(BaseVideoWriter&&)      = delete;
    BaseVideoWriter(const BaseVideoWriter&)            = delete;
    BaseVideoWriter(BaseVideoWriter&&)                 = delete;
    friend std::shared_ptr<BaseVideoWriter> createVideoWriter(WriterBackend backend, VWSettings settings);

protected:
    virtual void initImpl() = 0;
    VWSettings settings_;

private:
    SCREENCAPTURELIB_SUPPRESS_C4251
    static BaseVideoWriter* createVideoWriterImpl(WriterBackend backend, VWSettings settings);
};

inline std::shared_ptr<BaseVideoWriter> createVideoWriter(WriterBackend backend, VWSettings settings)
{
    return std::shared_ptr<BaseVideoWriter>(BaseVideoWriter::createVideoWriterImpl(backend, settings));
}

}
