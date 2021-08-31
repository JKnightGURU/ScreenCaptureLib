cmake_minimum_required(VERSION 3.14)

project(ScreenCaptureLibTests LANGUAGES CXX)

include(../cmake/project-is-top-level.cmake)
include(../cmake/windows-set-path.cmake)

if(PROJECT_IS_TOP_LEVEL)
  find_package(ScreenCaptureLib REQUIRED)
  enable_testing()
endif()

add_executable(ScreenCaptureLib_test source/ScreenCaptureLib_test.cpp)
target_link_libraries(ScreenCaptureLib_test PRIVATE ScreenCaptureLib::ScreenCaptureLib)
target_compile_features(ScreenCaptureLib_test PRIVATE cxx_std_17)

add_test(NAME ScreenCaptureLib_test COMMAND ScreenCaptureLib_test)
windows_set_path(ScreenCaptureLib_test ScreenCaptureLib::ScreenCaptureLib)
