cmake_minimum_required(VERSION 3.14)

include(cmake/prelude.cmake)

project(
    ScreenCaptureLib
    VERSION 0.1.0
    DESCRIPTION "Cross-platform C++ screen capture and recording library"
    HOMEPAGE_URL "https://github.com/jknightmmcs/ScreenCaptureLib"
    LANGUAGES CXX
)

option(BUILD_SHARED_LIBS "Build shared lib instead static one" ON)

docs_early_return()

include(cmake/project-is-top-level.cmake)
include(cmake/variables.cmake)

# ---- Declare dependencies ----
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

if(WIN32)
    include_directories(external/screen_capture_lite/include)
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

include_directories(
    src/ScreenCaptureLib
    src/ScreenCaptureLib/grabber_backends
    src/ScreenCaptureLib/writer_backends
)

target_link_libraries(ScreenCaptureLib_ScreenCaptureLib ${OpenCV_LIBS})

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
