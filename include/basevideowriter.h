#ifndef BASEVIDEOWRITER_H
#define BASEVIDEOWRITER_H

#include <opencv2/core.hpp>
#include <cstdint>
#include <memory>

#include "utils/scap_defs.h"

namespace sc_api {

struct VWSettings
{
    char fname_[512];
    int  fps_;
    int  width_, height_;
    int  bitrate_ = 800000;
};

class DLL_PREFIX BaseVideoWriter
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
    static BaseVideoWriter* createVideoWriterImpl(WriterBackend backend, VWSettings settings);
};

inline std::shared_ptr<BaseVideoWriter> createVideoWriter(WriterBackend backend, VWSettings settings)
{
    return std::shared_ptr<BaseVideoWriter>(BaseVideoWriter::createVideoWriterImpl(backend, settings));
}

}
#endif // BASEVIDEOWRITER_H
