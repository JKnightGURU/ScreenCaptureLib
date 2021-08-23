#include "mfvideowriter.h"
#include <iostream>

#pragma comment(lib, "mfreadwrite")
#pragma comment(lib, "mfplat")
#pragma comment(lib, "mfuuid")

namespace sc_api {

template<class T> void SafeRelease(T** ppT)
{
    if (*ppT)
    {
        (*ppT)->Release();
        *ppT = nullptr;
    }
}

void MFVideoWriter::write(cv::Mat mat)
{
    const LONG cbWidth = 3 * settings_.width_;

    BYTE* pData = nullptr;

    // Lock the buffer and copy the video frame to the buffer.
    HRESULT hr = buffer_->Lock(&pData, nullptr, nullptr);

    if (SUCCEEDED(hr))
    {
        for (int i = 0, j = settings_.height_ - 1; i < settings_.height_; i++, j--)
            for (int k = 0; k < cbWidth; k++)
                pData[(i * cbWidth) + k] = (reinterpret_cast<BYTE*>(mat.data))[(j * cbWidth) + k];
    }

    if (buffer_)
    {
        buffer_->Unlock();
    }

    // Set the time stamp and the duration.
    if (SUCCEEDED(hr))
    {
        hr         = sample_->SetSampleTime(rt_start_);
        rt_start_ += 10 * 1000 * 1000 / settings_.fps_;
    }

    if (SUCCEEDED(hr))
    {
        hr = sample_->SetSampleDuration(10LL * 1000LL * 1000LL / settings_.fps_);
    }

    // Send the sample to the Sink Writer.
    if (SUCCEEDED(hr))
    {
        hr = sink_writer_->WriteSample(stream_, sample_);

        // std::cout << "WriteSample hr = " << hr << std::endl;
    }
}

MFVideoWriter::~MFVideoWriter()
{
    sink_writer_->Finalize();
    SafeRelease(&buffer_);
    SafeRelease(&sample_);
    SafeRelease(&sink_writer_);
    MFShutdown();
    CoUninitialize();
}

void MFVideoWriter::initImpl()
{
    rt_start_ = 0;
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    MFStartup(MF_VERSION);

    initializeSinkWriter();
    initializeBuffer();
}

void MFVideoWriter::initializeSinkWriter()
{
    IMFMediaType* pMediaTypeOut = nullptr;
    IMFMediaType* pMediaTypeIn  = nullptr;

    sink_writer_ = nullptr;

    HRESULT hr = MFCreateSinkWriterFromURL(std::wstring(settings_.fname_, settings_.fname_ + strlen(settings_.fname_)).c_str(),
                                           nullptr,
                                           nullptr,
                                           &sink_writer_);

    // Set the output media type.
    if (SUCCEEDED(hr))
    {
        hr = MFCreateMediaType(&pMediaTypeOut);
    }

    if (SUCCEEDED(hr))
    {
        hr = pMediaTypeOut->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
    }

    if (SUCCEEDED(hr))
    {
        hr = pMediaTypeOut->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_H264);
    }

    if (SUCCEEDED(hr))
    {
        hr = pMediaTypeOut->SetUINT32(MF_MT_AVG_BITRATE, static_cast<UINT32>(settings_.bitrate_));
    }

    if (SUCCEEDED(hr))
    {
        hr = pMediaTypeOut->SetUINT32(MF_MT_INTERLACE_MODE, MFVideoInterlace_Progressive);
    }

    if (SUCCEEDED(hr))
    {
        hr =
            MFSetAttributeSize(pMediaTypeOut,
                               MF_MT_FRAME_SIZE,
                               static_cast<UINT32>(settings_.width_),
                               static_cast<UINT32>(settings_.height_));
    }

    if (SUCCEEDED(hr))
    {
        hr = MFSetAttributeRatio(pMediaTypeOut, MF_MT_FRAME_RATE, static_cast<UINT32>(settings_.fps_), 1);
    }

    if (SUCCEEDED(hr))
    {
        hr = MFSetAttributeRatio(pMediaTypeOut, MF_MT_PIXEL_ASPECT_RATIO, 1, 1);
    }

    if (SUCCEEDED(hr))
    {
        hr = sink_writer_->AddStream(pMediaTypeOut, &stream_);
    }

    // Set the input media type.
    if (SUCCEEDED(hr))
    {
        hr = MFCreateMediaType(&pMediaTypeIn);
    }

    if (SUCCEEDED(hr))
    {
        hr = pMediaTypeIn->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
    }

    if (SUCCEEDED(hr))
    {
        hr = pMediaTypeIn->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_RGB24);
    }

    if (SUCCEEDED(hr))
    {
        hr = pMediaTypeIn->SetUINT32(MF_MT_INTERLACE_MODE, MFVideoInterlace_Progressive);
    }

    if (SUCCEEDED(hr))
    {
        hr = MFSetAttributeSize(pMediaTypeIn,
                                MF_MT_FRAME_SIZE,
                                static_cast<UINT32>(settings_.width_),
                                static_cast<UINT32>(settings_.height_));
    }

    if (SUCCEEDED(hr))
    {
        hr = MFSetAttributeRatio(pMediaTypeIn, MF_MT_FRAME_RATE, static_cast<UINT32>(settings_.fps_), 1);
    }

    if (SUCCEEDED(hr))
    {
        hr = MFSetAttributeRatio(pMediaTypeIn, MF_MT_PIXEL_ASPECT_RATIO, 1, 1);
    }

    if (SUCCEEDED(hr))
    {
        hr = sink_writer_->SetInputMediaType(stream_, pMediaTypeIn, nullptr);
    }

    // Tell the sink writer to start accepting data.
    if (SUCCEEDED(hr))
    {
        hr = sink_writer_->BeginWriting();
        // std::cout << "BeginWriting: " << std::hex << hr << std::endl;
    }

    // Return the pointer to the caller.

    SafeRelease(&pMediaTypeOut);
    SafeRelease(&pMediaTypeIn);
}

void MFVideoWriter::initializeBuffer()
{
    // Create a new memory buffer.
    MFCreateMemoryBuffer(static_cast<DWORD>(3 * settings_.width_ * settings_.height_), &buffer_);

    // Set the data length of the buffer.
    buffer_->SetCurrentLength(static_cast<DWORD>(3 * settings_.width_ * settings_.height_));

    // Create a media sample and add the buffer to the sample.
    MFCreateSample(&sample_);

    sample_->AddBuffer(buffer_);
}

}
