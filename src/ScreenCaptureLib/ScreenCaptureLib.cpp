#include <string>

#include <ScreenCaptureLib/ScreenCaptureLib.hpp>

exported_class::exported_class()
    : m_name("ScreenCaptureLib")
{
}

auto exported_class::name() const -> const char*
{
  return m_name.c_str();
}
