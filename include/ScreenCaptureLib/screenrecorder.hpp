#pragma once

#include <string>
#include <vector>

#include "ScreenCaptureLib/defs/scap_defs.hpp"
#include "ScreenCaptureLib/ScreenCaptureLib_export.h"

namespace sc_api {

class ScreenRecorderImpl;

class SCREENCAPTURELIB_EXPORT ScreenRecorder
{
public:
    ScreenRecorder();
    ~ScreenRecorder();

    ///
    /// \brief init Function initializes screen reading utilities, and sets up circular buffer for saved screenshots
    /// \param buffer_sec_size Buffer size in seconds
    /// \param fps desired FPS for created video
    ///
    void init(int buffer_sec_size = 5, int fps = 10);

    ///
    /// \brief startRecording Starts video writing utilities, flushes circular buffer, then writes every incoming frame
    /// \param output_path Path to video, e.g. /home/bullshit/videos
    /// \param name_prefix Prefix for created video name, e.g. "brazzers_home_video_" -> "/home/bullshit/videos/brazzers_home_video_0.mp4"
    /// \param backend Backend used to create a video
    ///
    void startRecording(const char* output_path, const char* name_prefix, WriterBackend backend = wbFFMPEG);

    ///
    /// \brief stopRecording Stop video writing, finalizes video file.
    ///
    void stopRecording();

    ///
    /// \brief version Returns current version.
    ///
    uint32_t version();

private:
    SCREENCAPTURELIB_SUPPRESS_C4251
    ScreenRecorderImpl* impl_;
};

}
