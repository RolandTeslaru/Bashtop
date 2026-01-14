#include <cstdlib>
#include <iostream>

#include "monitor/core/App.hpp"
#include "monitor/core/TestsRunner.hpp"
#include "monitor/exceptions/Platform.hpp"

using PlatformNotSupportedException =
    monitor::exceptions::PlatformNotSupportedException;

int main() {
#if !defined(__linux__) && !defined(__APPLE__) && !defined(_WIN32)
  throw PlatformNotSupportedException("This os is not supported yet.");
#endif
  char a = '1';

  if (const char *no_prompt = std::getenv("BASHTOP_NO_PROMPT")) {
    (void)no_prompt;
  } else {
    std::cout << "Enter 0 to run tests, or anything else to start the monitor:"
              << std::endl;
    if (!(std::cin >> a)) {
      a = '1';
    }
  }

  if (a == '0') {
    monitor::TestsRunner::getInstance().run();
    return 0;
  }

  auto &appInstance = monitor::App::getInstance();

  appInstance.startFrameLoop();
  appInstance.shutdown();

  return 0;
}