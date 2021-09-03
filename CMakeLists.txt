cmake_minimum_required(VERSION 3.14)

include(cmake/prelude.cmake)

project(
    ScreenCaptureLib
    VERSION 0.1.0
    DESCRIPTION "Cross-platform C++ screen capture and recording library"
    HOMEPAGE_URL "https://github.com/jknightmmcs/ScreenCaptureLib"
    LANGUAGES CXX
)

docs_early_return()

include(cmake/project-is-top-level.cmake)
include(cmake/variables.cmake)

# ---- Declare dependencies ----
find_package(OpenCV REQUIRED)
find_package(Boost 1.37.0 REQUIRED)

# FFMPEG doesn't have nation cmake support
# So instead of find_package we use find_path + find_library calls 
find_path(AVCODEC_INCLUDE_DIR libavcodec/avcodec.h)
find_library(AVCODEC_LIBRARY avcodec)

find_path(AVFORMAT_INCLUDE_DIR libavformat/avformat.h)
find_library(AVFORMAT_LIBRARY avformat)

find_path(AVUTIL_INCLUDE_DIR libavutil/avutil.h)
find_library(AVUTIL_LIBRARY avutil)

find_path(SWSCALE_INCLUDE_DIR libswscale/swscale.h)
find_library(SWSCALE_LIBRARY swscale)

if(WIN32)
    find_path(SC_LITE_INCLUDE_DIR ScreenCapture.h HINTS external/screen_capture_lite/include)
	find_library(SC_LITE_LIBRARY_DEBUG screen_capture_lite_d HINTS external/screen_capture_lite/lib)
	find_library(SC_LITE_LIBRARY_RELEASE screen_capture_lite HINTS external/screen_capture_lite/lib)
endif(WIN32)

# ---- Declare library ----

set(GrabberBackend_cxx_files "")
set(WriterBackend_cxx_files
    src/ScreenCaptureLib/writer_backends/basevideowriter.cpp
    src/ScreenCaptureLib/writer_backends/cvvideowriter.cpp
    src/ScreenCaptureLib/writer_backends/ffmpegvideowriter.cpp)

if(UNIX AND NOT APPLE)
    set(GrabberBackend_cxx_files
        ${GrabberBackend_cxx_files}
        src/ScreenCaptureLib/grabber_backends/xshmgrabber.cpp)
endif(UNIX AND NOT APPLE)

if(APPLE)

endif(APPLE)

if(WIN32)
    set(GrabberBackend_cxx_files
        ${GrabberBackend_cxx_files}
        src/ScreenCaptureLib/grabber_backends/sclitegrabber.cpp)

    set(WriterBackend_cxx_files
        ${WriterBackend_cxx_files}
        src/ScreenCaptureLib/writer_backends/mfvideowriter.cpp)
endif(WIN32)

add_library(
    ScreenCaptureLib_ScreenCaptureLib
    src/ScreenCaptureLib/ScreenCaptureLib.cpp
    src/ScreenCaptureLib/screenrecorder.cpp
    ${GrabberBackend_cxx_files}
    ${WriterBackend_cxx_files}
)
add_library(ScreenCaptureLib::ScreenCaptureLib ALIAS ScreenCaptureLib_ScreenCaptureLib)

include(GenerateExportHeader)
generate_export_header(
    ScreenCaptureLib_ScreenCaptureLib
    BASE_NAME ScreenCaptureLib
    EXPORT_FILE_NAME export/ScreenCaptureLib/ScreenCaptureLib_export.h
    CUSTOM_CONTENT_FROM_VARIABLE pragma_suppress_c4251
)

if(NOT BUILD_SHARED_LIBS)
  target_compile_definitions(ScreenCaptureLib_ScreenCaptureLib PUBLIC SCREENCAPTURELIB_STATIC_DEFINE)
endif()

set_target_properties(
    ScreenCaptureLib_ScreenCaptureLib PROPERTIES
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN YES
    VERSION "${PROJECT_VERSION}"
    SOVERSION "${PROJECT_VERSION_MAJOR}"
    EXPORT_NAME ScreenCaptureLib
    OUTPUT_NAME ScreenCaptureLib
)

# --- Compiler setup ---
if(MSVC)
    target_compile_options(ScreenCaptureLib_ScreenCaptureLib PRIVATE /W3 /WX /wd4996)
else()
    target_compile_options(ScreenCaptureLib_ScreenCaptureLib PRIVATE -Wno-deprecated-declarations -Werror)
endif()

target_include_directories(
    ScreenCaptureLib_ScreenCaptureLib ${ScreenCaptureLib_warning_guard}
    PUBLIC
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
)

target_include_directories(
    ScreenCaptureLib_ScreenCaptureLib SYSTEM
    PUBLIC
    "$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/export>"
)

target_include_directories(
    ScreenCaptureLib_ScreenCaptureLib
	PRIVATE
    src/ScreenCaptureLib
    src/ScreenCaptureLib/grabber_backends
    src/ScreenCaptureLib/writer_backends
	${OpenCV_INCLUDE_DIRS}
	${Boost_INCLUDE_DIRS}
	${AVCODEC_INCLUDE_DIR}
	${AVFORMAT_INCLUDE_DIR}
	${AVUTIL_INCLUDE_DIR}
	${SWSCALE_INCLUDE_DIR}
	${SC_LITE_INCLUDE_DIR}
)

target_link_libraries(ScreenCaptureLib_ScreenCaptureLib 
    ${OpenCV_LIBS}
	${AVCODEC_LIBRARY}
	${AVFORMAT_LIBRARY}
	${AVUTIL_LIBRARY}
	${SWSCALE_LIBRARY}
	debug ${SC_LITE_LIBRARY_DEBUG}
	optimized ${SC_LITE_LIBRARY_RELEASE})

target_compile_features(ScreenCaptureLib_ScreenCaptureLib PUBLIC cxx_std_17)

# ---- Install rules ----

if(NOT CMAKE_SKIP_INSTALL_RULES)
  include(cmake/install-rules.cmake)
endif()

# ---- Developer mode ----

if(NOT ScreenCaptureLib_DEVELOPER_MODE)
  return()
elseif(NOT PROJECT_IS_TOP_LEVEL)
  message(
      AUTHOR_WARNING
      "Developer mode is intended for developers of ScreenCaptureLib"
  )
endif()

include(cmake/dev-mode.cmake)
