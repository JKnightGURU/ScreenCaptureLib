#ifndef SCAP_DEFS_H
#define SCAP_DEFS_H

#define AL_LIB_NAME ScreenCaptureLib
#define AL_ID SC

#include "utils/lib_dll_helper.h"

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
#endif // SCAP_DEFS_H
