#pragma once

#include <memory>

namespace monitor {

class TestsRunner {
public:
  static TestsRunner &getInstance();

  TestsRunner(const TestsRunner &) = delete;
  TestsRunner &operator=(const TestsRunner &) = delete;
  TestsRunner(TestsRunner &&) = delete;
  TestsRunner &operator=(TestsRunner &&) = delete;

  void run();

private:
  TestsRunner() = default;
};

}
