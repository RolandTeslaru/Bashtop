#pragma once

#include "monitor/core/Engine.hpp"
#include "monitor/metrics/PlatformInfoMonitor.hpp"
#include "monitor/ui/Menubar.hpp"
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

namespace monitor {
class App {
public:
  static App &getInstance();
  [[maybe_unused]] static Engine &getEngine();

  App(const App &other) = delete;
  App &operator=(const App &) = delete;

  void startFrameLoop();
  void shutdown();

private:
  App();
  ~App() = default;
  Engine engine;
  monitor::metrics::PlatformInfoMonitor platformInfoMonitor;
  ftxui::ScreenInteractive screen = ftxui::ScreenInteractive::Fullscreen();

  std::shared_ptr<monitor::ui::MenubarWidget> Menubar;
  ftxui::Component WidgetsContentArea;
  ftxui::Component root;
};
} // namespace monitor