#include <iostream>
#include <chrono>
#include <thread>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
// #include <ftxui/component/>

#include <thread>

#include <cmath>
#include "monitor/metrics/CpuMonitor.hpp"
#include "monitor/metrics/SystemInfoProvider.hpp"
#include "monitor/os/Factory.hpp"
#include "monitor/core/Engine.hpp"

using namespace ftxui;

using CpuMonitor    = monitor::metrics::CpuMonitor;
using vector_double = std::vector<double>;
using vector_int    = std::vector<int>;


namespace monitor::ui
{
    class CpuWidget : public ComponentBase
    {
    public:
        explicit CpuWidget(CpuMonitor &monitor_) : monitor(monitor_) {}

        Element OnRender() override
        {
            double current_total_usage = monitor.getCpuTotalUsage();
            cpu_usage_history = monitor.getCpuUsageHistory();

            auto graphFunc = [this](int width, int height)
            {
                vector_int out(width, 0);

                int n = (int)cpu_usage_history.size();
                int start = n > width ? n - width : 0;

                for (int x = 0; x < width; ++x)
                {
                    int idx = start + x;
                    if (idx >= n)
                        break;
                    double v = cpu_usage_history[idx];
                    int h = static_cast<int>(std::round(v / 100.0 * (height - 1)));
                    if (h < 0)
                        h = 0;
                    if (h >= height)
                        h = height - 1;
                    out[x] = h;
                }
                return out;
            };
            return window(
                text("CPU"),
                hbox({
                    vbox({
                        text("Total: " + std::to_string(current_total_usage) + "%") | bold,
                        separator(),
                        graph(graphFunc) | color(Color::RedLight),
                    }) | xflex,
                    separator(),
                    vbox({
                        text("Cores:") | bold,
                        separator(),

                        vbox([this]() {
                            Elements core_elems;
                            int num_cores = monitor.getNumCores();
                            for (int i = 0; i < num_cores; ++i)
                            {
                                int core_usage = (int)monitor.getCpuCoreUsage(i);
                                core_elems.push_back(
                                    text("Core " + std::to_string(i) + ": " + std::to_string(core_usage) + "%"));
                            }
                            return vbox(core_elems);
                        }()),
                    }) | size(WIDTH, EQUAL, 20)
                }) | flex
            );
        }

    private:
        CpuMonitor                 &monitor;
        vector_double              cpu_usage_history;
        std::vector<vector_double> core_usage_history;
    };
}



using namespace ftxui;



int main()
{
    // For tests.
    int a;

    std::cout << "Enter anything BUT 0 to start: " << std::endl;

    std::cin >> a;
    if (a == 0)
        return 0;


    // Not used, but still call make_platform_info so cppcheck doesnt yap about unused function
    auto platformInfo = monitor::metrics::SystemInfoProvider(
        monitor::os::make_platform_info()
    );
    
    monitor::Engine engine;
    auto &cpuMonitor = engine.getCpuMonitor();
        
    auto screen = ScreenInteractive::Fullscreen();
    
    engine.ignition(screen);

    auto cpuWidget = Make<monitor::ui::CpuWidget>(cpuMonitor);
    Component root = Renderer(cpuWidget, [cpuWidget] {
        return hbox({
            cpuWidget->Render() | ftxui::flex,
        }) | ftxui::flex;
    });

    screen.Loop(root);
    engine.shutdown();

    return 0;
}