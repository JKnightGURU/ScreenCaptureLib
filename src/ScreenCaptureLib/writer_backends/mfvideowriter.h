#pragma once

#include "ScreenCaptureLib/basevideowriter.hpp"

#include <Windows.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <Mferror.h>

namespace sc_api {

class MFVideoWriter : public BaseVideoWriter
{
public:
    virtual void write(cv::Mat mat) override;
    ~MFVideoWriter();

protected:
    virtual void initImpl() override;

private:
    void initializeSinkWriter();
    void initializeBuffer();

    IMFMediaBuffer* buffer_ = nullptr;
    IMFSample* sample_      = nullptr;
    IMFSinkWriter* sink_writer_;
    DWORD stream_;
    LONGLONG rt_start_;
};

}
