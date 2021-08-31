#include <string>

#include <ScreenCaptureLib/ScreenCaptureLib.hpp>

auto main() -> int
{
  exported_class e;

  return std::string("ScreenCaptureLib") == e.name() ? 0 : 1;
}
