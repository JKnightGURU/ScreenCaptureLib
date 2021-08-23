//------------------------------------------------------------------------------------------------------------------------------------------
//    Copyright (c) 2011, Eugene Mamin <TheDZhon@gmail.com>
//    All rights reserved.
//
//    Redistribution and use in source and binary forms, with or without
//    modification, are permitted provided that the following conditions are met:
//        * Redistributions of source code must retain the above copyright
//        notice, this list of conditions and the following disclaimer.
//        * Redistributions in binary form must reproduce the above copyright
//        notice, this list of conditions and the following disclaimer in the
//        documentation and/or other materials provided with the distribution.
//        * Neither the name of the Prefix Increment nor the
//        names of its contributors may be used to endorse or promote products
//        derived from this software without specific prior written permission.
//
//    THIS SOFTWARE IS PROVIDED BY Eugene Mamin <TheDZhon@gmail.com> ''AS IS'' AND ANY
//    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
//    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
//    DISCLAIMED. IN NO EVENT SHALL Eugene Mamin <TheDZhon@gmail.com> BE LIABLE FOR ANY
//    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
//    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OALSUBSTITUTE GOODS OR SERVICES;
//    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
//    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------------------------------------------------------------------

/**
 ** !!! INTERNAL HEADER !!!
 ** DON'T USE IT INSIDE YOUR PROJECTS!
 **/

/**
 ** Auto link header for automatic library insertion into MSVC.
 ** Using code from boost/config/auto_link.hpp (c) John Maddock, 2003
 **/

#include "lib_dll_helper.h"

#define AL_NAME_TO_CSTR(X) DO_AL_NAME_TO_CSTR(X)
#define DO_AL_NAME_TO_CSTR(X) #X

#define ADD_DISABLE_SUFFIX(X) DO_ADD_DISABLE_SUFFIX(X)
#define DO_ADD_DISABLE_SUFFIX(X) X##_DISABLE_AUTOLINK

#define ADD_EXPORT_DLL_SUFFIX(X) DO_ADD_EXPORT_DLL_SUFFIX(X)
#define DO_ADD_EXPORT_DLL_SUFFIX(X) X##_EXPORT_DLL

#define AL_DISABLE_AUTOLINK ADD_DISABLE_SUFFIX(AL_ID)
#define AL_EXPORT_DLL ADD_EXPORT_DLL_SUFFIX(AL_ID)

// Currently supports only MSVC >= 7.0
#if defined(_MSC_VER) && defined(_WIN32) && (_MSC_VER >= 1300)  \
	&& !AL_EXPORT_DLL && !AL_DISABLE_AUTOLINK

#ifndef AL_LIB_NAME
#error "AL_LIB_NAME macro var is empty! Define it before using this header"
#endif

// Detect MSVS version
#if defined(_MSC_VER) && (_MSC_VER == 1500) // Visual Studio 2008
// vc90:
#  define AL_LIB_TOOLSET "v90"

#elif defined(_MSC_VER) && (_MSC_VER == 1600) // Visual Studio 2010
// vc100:
#  define AL_LIB_TOOLSET "v100"

#elif defined(_MSC_VER) && (_MSC_VER == 1700) // Visual Studio 2012
// vc110:
#  define AL_LIB_TOOLSET "v110"
#endif

// Detect platform
#if defined (_WIN64)
#define AL_LIB_ARCH "amd64"
#else
#define AL_LIB_ARCH "x86"
#endif

#if defined(__MSVC_RUNTIME_CHECKS) && !defined(_DEBUG)
#pragma message("Using the /RTC option without specifying a debug runtime will lead to linker errors")
#pragma message("Hint: go to the code generation options and switch to one of the debugging runtimes")
#error "Incompatible build options"
#endif

#if defined(_DEBUG)
#define AL_LIB_RT_OPT "_d"
#else
#define AL_LIB_RT_OPT
#endif

#if !defined(AL_LIB_RT_OPT) && !defined(AL_LIB_NAME)
#error "Internal logic error"
#endif

#ifdef AL_ENABLE_LINK_DIAGNOSTIC
#pragma message ("Auto linking to lib file: " AL_NAME_TO_CSTR(AL_LIB_NAME) AL_LIB_RT_OPT ".lib")
#endif
#pragma comment(lib, AL_NAME_TO_CSTR(AL_LIB_NAME) AL_LIB_RT_OPT ".lib")

#endif // End of body

#ifdef AL_LIB_NAME
#undef AL_LIB_NAME
#endif

#ifdef AL_ID
#undef AL_ID
#endif

#ifdef AL_LIB_RT_OPT
#undef AL_LIB_RT_OPT
#endif