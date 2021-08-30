if(PROJECT_IS_TOP_LEVEL)
  set(CMAKE_INSTALL_INCLUDEDIR include/ScreenCaptureLib CACHE PATH "")
endif()

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

# find_package(<package>) call for consumers to find this project
set(package ScreenCaptureLib)

install(
    DIRECTORY
    include/
    "${PROJECT_BINARY_DIR}/export/"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    COMPONENT ScreenCaptureLib_Development
)

install(
    TARGETS ScreenCaptureLib_ScreenCaptureLib
    EXPORT ScreenCaptureLibTargets
    RUNTIME #
    COMPONENT ScreenCaptureLib_Runtime
    LIBRARY #
    COMPONENT ScreenCaptureLib_Runtime
    NAMELINK_COMPONENT ScreenCaptureLib_Development
    ARCHIVE #
    COMPONENT ScreenCaptureLib_Development
    INCLUDES #
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

write_basic_package_version_file(
    "${package}ConfigVersion.cmake"
    COMPATIBILITY SameMajorVersion
)

# Allow package maintainers to freely override the path for the configs
set(
    ScreenCaptureLib_INSTALL_CMAKEDIR "${CMAKE_INSTALL_DATADIR}/${package}"
    CACHE PATH "CMake package config location relative to the install prefix"
)
mark_as_advanced(ScreenCaptureLib_INSTALL_CMAKEDIR)

install(
    FILES cmake/install-config.cmake
    DESTINATION "${ScreenCaptureLib_INSTALL_CMAKEDIR}"
    RENAME "${package}Config.cmake"
    COMPONENT ScreenCaptureLib_Development
)

install(
    FILES "${PROJECT_BINARY_DIR}/${package}ConfigVersion.cmake"
    DESTINATION "${ScreenCaptureLib_INSTALL_CMAKEDIR}"
    COMPONENT ScreenCaptureLib_Development
)

install(
    EXPORT ScreenCaptureLibTargets
    NAMESPACE ScreenCaptureLib::
    DESTINATION "${ScreenCaptureLib_INSTALL_CMAKEDIR}"
    COMPONENT ScreenCaptureLib_Development
)

if(PROJECT_IS_TOP_LEVEL)
  include(CPack)
endif()
