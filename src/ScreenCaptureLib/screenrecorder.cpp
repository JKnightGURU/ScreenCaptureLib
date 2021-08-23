#include "screenrecorder.h"

#include <iostream>
#include <mutex>
#include <thread>
#include <atomic>

#include "wisdom/circular_buffer.h"

#include "basevideowriter.h"
#include "cvvideowriter.h"
#include "ffmpegvideowriter.h"

#include "basegrabber.h"
#if defined(WIN32)
# include "mfvideowriter.h"
# include "sclitegrabber.h"
#elif defined(__linux__)
# include "xshmgrabber.h"
#endif // if defined(WIN32)

namespace sc_api {

class RecordState
{
public:
    RecordState() :
        RecordState(5, 10) {}
    RecordState(int buffer_size_sec, int fps) :
        is_stop_write_(std::make_shared<std::atomic<bool>>(false)),
        buffer_(static_cast<size_t>(buffer_size_sec * fps)),
        fps_(fps)
    {}
    RecordState(const RecordState&) = delete;
    void update();
    void swap(cv::Mat mat);

    friend class ScreenRecorderImpl;

private:
    cv::Mat mat_;
    std::mutex mutex_write_;
    std::shared_ptr<BaseVideoWriter> writer_;
    std::thread write_buf_thr_;
    std::shared_ptr<std::atomic<bool>> is_stop_write_;
    wisdom::CircularBuffer<cv::Mat> buffer_;
    int fps_;
    int width_, height_;
};

class ScreenRecorderImpl
{
public:
    ScreenRecorderImpl() {}
    ~ScreenRecorderImpl();
    void init(int buffer_sec_size              = 5, int fps= 10);

    void startRecording(const char* output_path, const char* name_prefix, WriterBackend backend = wbOpenCV);
    void stopRecording();

private:
    std::shared_ptr<BaseGrabber> grabber_;
    int fps_;
    std::vector<std::shared_ptr<RecordState>> monitors_;

    bool is_recording_;
};


ScreenRecorderImpl::~ScreenRecorderImpl()
{
    grabber_.reset();
    stopRecording();
}

void ScreenRecorderImpl::init(int buffer_sec_size, int fps)
{
    fps_          = fps;
    is_recording_ = false;

#if defined(WIN32)
    grabber_ = std::make_shared<SCLiteGrabber>();
#elif defined(__linux__)
    grabber_ = std::make_shared<XShmGrabber>();
#endif // if defined(WIN32)

    auto monitors = grabber_->monitors();
    monitors_.clear();

    for (size_t i = 0; i < monitors.size(); i++)
    {
        monitors_.push_back(std::make_shared<RecordState>(buffer_sec_size, fps));
        monitors_.back()->width_  = monitors[i].width_;
        monitors_.back()->height_ = monitors[i].height_;
    }
    grabber_->init([this](cv::Mat mat, int mon_index) {
                       std::lock_guard<std::mutex>(monitors_[static_cast<size_t>(mon_index)]->mutex_write_);
                       monitors_[static_cast<size_t>(mon_index)]->buffer_.add(mat);
                   }, fps);
}

void ScreenRecorderImpl::startRecording(const char* output_path, const char* name_prefix, WriterBackend backend)
{
    if (is_recording_)
        return;

    for (size_t i = 0; i < monitors_.size(); i++)
    {
        VWSettings settings;
        settings.bitrate_ = 800000;
        settings.fps_     = fps_;
        settings.width_   = monitors_[i]->width_;
        settings.height_  = monitors_[i]->height_;

        std::string fname = std::string(output_path) + "//" + std::string(name_prefix) + std::to_string(i);

        if (backend == wbOpenCV)
            fname += ".avi";
        else
            fname += ".mp4";

        settings.fname_[fname.copy(settings.fname_, sizeof(settings.fname_))] = '\0';

        monitors_[i]->writer_         = createVideoWriter(backend, settings);
        *monitors_[i]->is_stop_write_ = false;

        monitors_[i]->write_buf_thr_ =
            std::thread([i, this]() {
                            while (!*monitors_[i]->is_stop_write_)
                            {
                                if (monitors_[i]->buffer_.queue_size() == 0)
                                {
                                    std::this_thread::sleep_for(std::chrono::milliseconds(200));
                                }
                                else
                                {
                                    while ((monitors_[i]->buffer_.queue_size() != 0) &&
                                           (!*monitors_[i]->is_stop_write_))
                                    {
                                        std::lock_guard<std::mutex>(monitors_[i]->mutex_write_);
                                        cv::Mat img = monitors_[i]->buffer_.front();
                                        monitors_[i]->writer_->write(img);
                                        monitors_[i]->buffer_.pop();
                                    }

                                    if (*monitors_[i]->is_stop_write_)
                                    {
                                        std::lock_guard<std::mutex>(monitors_[i]->mutex_write_);

                                        while (monitors_[i]->buffer_.queue_size() != 0)
                                        {
                                            cv::Mat img = monitors_[i]->buffer_.front();
                                            monitors_[i]->writer_->write(img);
                                            monitors_[i]->buffer_.pop();
                                        }
                                    }
                                }
                            }
                        });
    }

    is_recording_ = true;
}

void ScreenRecorderImpl::stopRecording()
{
    if (!is_recording_)
        return;

    for (size_t i = 0; i < monitors_.size(); i++)
    {
        if (monitors_[i]->writer_)
        {
            *monitors_[i]->is_stop_write_ = true;
            monitors_[i]->write_buf_thr_.join();
            monitors_[i]->writer_.reset();
        }
    }

    is_recording_ = false;
}

ScreenRecorder::ScreenRecorder()
{
    impl_ = new ScreenRecorderImpl();
}

ScreenRecorder::~ScreenRecorder()
{
    delete impl_;
}

void ScreenRecorder::init(int buffer_sec_size, int fps)
{
    impl_->init(buffer_sec_size, fps);
}

void ScreenRecorder::startRecording(const char* output_path, const char* name_prefix, WriterBackend backend)
{
    impl_->startRecording(output_path, name_prefix, backend);
}

void ScreenRecorder::stopRecording()
{
    impl_->stopRecording();
}

uint32_t ScreenRecorder::version()
{
    return SCREENCAPTURE_VERSION;
}

}
