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
//    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
//    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
//    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------------------------------------------------------------------

#ifndef LIB_DLL_HELPER_H__
#define LIB_DLL_HELPER_H__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#define ADD_EXPORT_SUFFIX(X) DO_ADD_EXPORT_SUFFIX(X)
#define DO_ADD_EXPORT_SUFFIX(X) X##_EXPORT_DLL

#define EXPORT_DLL ADD_EXPORT_SUFFIX(AL_ID)

#if defined _WIN32 || defined __CYGWIN__ || defined _WIN64
#if EXPORT_DLL
#ifdef __GNUC__
#define DLL_PREFIX __attribute__ ((dllexport))
//#define __cdecl __attribute__((cdecl)) ; Already defined in CYGWIN
#else
#define DLL_PREFIX __declspec(dllexport) // Note: actually gcc seems to also supports this syntax.
#endif
#else
#ifdef __GNUC__
// #define __cdecl __attribute__((cdecl)) ; Already defined in CYGWIN
#define DLL_PREFIX __attribute__ ((dllimport))
#else
#define DLL_PREFIX __declspec(dllimport) // Note: actually gcc seems to also supports this syntax.
#endif
#endif
#define DLL_HIDE
#if defined _WIN64
#define __cdecl
#endif
#else
#ifndef __LP64__
#define __cdecl __attribute__((cdecl))
#define __stdcall __attribute__((stdcall))
#else
#define __cdecl
#define __stdcall
#endif
#if __GNUC__ >= 4
#define DLL_PREFIX __attribute__ ((visibility ("default")))
#define DLL_HIDE  __attribute__ ((visibility ("hidden")))
#else
#define DLL_PREFIX
#define DLL_HIDE
#endif
#endif

#ifdef STATIC_BUILD
#undef DLL_PREFIX
#define DLL_PREFIX
#endif

#include <cstddef>
#include <cstdint>

static const size_t kPatchOffset = 8;

inline bool checkAPI (uint32_t needed, uint32_t current)
{
	return (needed >> kPatchOffset) == (current >> kPatchOffset);
}

#endif // LIB_DLL_HELPER_H__
