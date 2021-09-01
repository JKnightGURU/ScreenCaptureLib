#pragma once

namespace sc_api {

enum WriterBackend
{
#ifdef WIN32
    wbMicrosoftMF,
#endif // ifdef WIN32
    wbOpenCV,
    wbFFMPEG
};

}
