#include "ScreenCaptureLib/basevideowriter.hpp"

#include "cvvideowriter.h"
#include "ffmpegvideowriter.h"
#ifdef _WIN32
# include "mfvideowriter.h"
#endif // ifdef _WIN32

namespace sc_api {

void BaseVideoWriter::init(VWSettings sets)
{
    settings_ = sets;
    initImpl();
}

BaseVideoWriter* BaseVideoWriter::createVideoWriterImpl(WriterBackend backend, VWSettings settings)
{
    BaseVideoWriter* writer;

    switch (backend)
    {
#ifdef _WIN32
    case wbMicrosoftMF:
        writer = new MFVideoWriter();
        break;
#endif // ifdef _WIN32
    case wbFFMPEG:
        writer = new FFmpegVideoWriter();
        break;
    case wbOpenCV:
        writer = new CVVideoWriter();
        break;
    }

    writer->init(settings);

    return writer;
}

}
