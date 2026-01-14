#include "monitor/core/TestsRunner.hpp"

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <iostream>

#include "monitor/ansi.hpp"
#include "monitor/core/Engine.hpp"
#include "monitor/metrics/CpuMonitor.hpp"
#include "monitor/metrics/PlatformInfoMonitor.hpp"
#include "monitor/os/Factory.hpp"
#include "monitor/os/simulator/CpuReaderSimulator.hpp"
#include "monitor/ui/CpuWidget.hpp"

using namespace ftxui; 
using CpuWidget = monitor::ui::CpuWidget;
using PlatformInfoMonitor = monitor::metrics::PlatformInfoMonitor;

namespace monitor {

TestsRunner &TestsRunner::getInstance() {
  static TestsRunner instance;
  return instance;
}

void TestsRunner::run() {
  std::cout << std::endl
            << monitor::ansi::BOLD << monitor::ansi::YELLOW
            << "Running tests..." << monitor::ansi::RESET << std::endl
            << std::endl;

  auto platformInfo =
      PlatformInfoMonitor(monitor::os::make_platform_info_reader());
  std::cout << platformInfo << std::endl;

  auto cpuReader = std::make_unique<monitor::os::simulator::CpuReader>();
  auto memReader = monitor::os::make_mem_reader();

  monitor::Engine engine(std::move(cpuReader), std::move(memReader));
  auto cpuMonitorPtr = engine.getCpuMonitor();

  // Compute a few snapshots to populate history (uses a simulation reader so
  // the data is fake)
  cpuMonitorPtr->computeSnapshot();
  cpuMonitorPtr->computeSnapshot();
  cpuMonitorPtr->computeSnapshot();

  std::cout << engine << std::endl;

  auto screen = ScreenInteractive::Fullscreen();

  auto cpuWidget = ftxui::Make<CpuWidget>(*cpuMonitorPtr);

  Component root = Renderer(cpuWidget, [cpuWidget] {
    return hbox({
               cpuWidget->Render() | ftxui::flex,
           }) |
           ftxui::flex;
  });

  std::cout << *cpuWidget << std::endl; // widget is a shared ptr so deref
}

} 
