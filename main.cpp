#include <iostream>
#include <chrono>
#include <thread>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
// #include <ftxui/component/>

#include <cmath>
#include "monitor/metrics/CpuMonitor.hpp"
#include "monitor/metrics/SystemInfoProvider.hpp"
#include "monitor/os/Factory.hpp"
#include "monitor/core/Engine.hpp"

using namespace ftxui;

class Graph {
    public:
    std::vector<int> operator()(int width, int height) const {
        std::vector<int> output(width);

        for(int i = 0; i < width; ++i){
            float v = 0;
            v += 0.1f * sin((i + shift) * 0.1f);        // NOLINT
            v += 0.2f * sin((i + shift + 10) * 0.15f);  // NOLINT
            v += 0.1f * sin((i + shift) * 0.03f);       // NOLINT
            v *= height;                                // NOLINT
            v += 0.5f * height;                         // NOLINT
            output[i] = static_cast<int>(v);
        }
        return output;
    }
    int shift = 0;
};

std::vector<int> triangle(int width, int height) {
  std::vector<int> output(width);
  for (int i = 0; i < width; ++i) {
    output[i] = i % (height - 4) + 2;
  }
  return output;
}




namespace monitor::ui{
    class CpuWidget : public ComponentBase {
        public: 
            CpuWidget(){}

            Element Render() override {
                return window(text("CPU"), vbox({
                    text("Total: " + std::to_string(monitor.getCpuTotalUsage())) | bold
                }));
            }
        private:
            monitor::metrics::CpuMonitor& monitor = monitor::engine.getCpuMonitor();
    };
}


// int main()
// {

//     Graph my_graph;
//     auto cpuMonitor = monitor::metrics::CpuMonitor(
//         monitor::os::make_cpu_reader()
//     );
//     auto platformInfo = monitor::metrics::SystemInfoProvider(
//         monitor::os::make_platform_info()
//     );

//     std::cout << platformInfo.getPlatformInfo() << std::endl;

//     // while(true){
//     //     cpuMonitor.computeSnapshot();
//     //     std::cout << "CPU total usage: " << cpuMonitor.getCpuTotalUsage() << "%" << std::endl << std::endl;
//     //     std::cout << cpuMonitor << std::endl << std::endl;
//     //     std::this_thread::sleep_for(std::chrono::seconds(2));
//     // }

//     return 0;
// }


int main() {
  using namespace ftxui;
  using namespace std::chrono_literals;
 
  Graph my_graph;
 
  std::string reset_position;
  for (int i = 0;; ++i) {
    auto document = hbox({
        vbox({
            graph(std::ref(my_graph)),
            separator(),
            graph(triangle) | inverted,
        }) | flex,
        separator(),
        vbox({
            graph(std::ref(my_graph)) | color(Color::BlueLight),
            separator(),
            graph(std::ref(my_graph)) | color(Color::RedLight),
            separator(),
            graph(std::ref(my_graph)) | color(Color::YellowLight),
        }) | flex,
    });
 
    document |= border;
 
    const int min_width = 40;
    document |= size(HEIGHT, GREATER_THAN, min_width);
 
    auto screen = Screen::Create(Dimension::Full(), Dimension::Fit(document));
    Render(screen, document);
    std::cout << reset_position;
    screen.Print();
    reset_position = screen.ResetPosition();
 
    const auto sleep_time = 0.03s;
    std::this_thread::sleep_for(sleep_time);
    my_graph.shift++;
  }
 
  return 0;
}