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

class Graph
{
public:
    std::vector<int> operator()(int width, int height) const
    {
        std::vector<int> output(width);

        for (int i = 0; i < width; ++i)
        {
            float v = 0;
            v += 0.1f * sin((i + shift) * 0.1f);       // NOLINT
            v += 0.2f * sin((i + shift + 10) * 0.15f); // NOLINT
            v += 0.1f * sin((i + shift) * 0.03f);      // NOLINT
            v *= height;                               // NOLINT
            v += 0.5f * height;                        // NOLINT
            output[i] = static_cast<int>(v);
        }
        return output;
    }
    int shift = 0;
};

std::vector<int> triangle(int width, int height)
{
    std::vector<int> output(width);
    for (int i = 0; i < width; ++i)
    {
        output[i] = i % (height - 4) + 2;
    }
    return output;
}

namespace monitor::ui
{
    // class CpuWidget : public ComponentBase
    // {
    // public:
    //     CpuWidget(monitor::metrics::CpuMonitor &_monitor)
    //         : monitor(_monitor) {}

    //     Element Render() override
    //     {
    //         return window(text("CPU"), vbox({text("Total: " + std::to_string(monitor.getCpuTotalUsage())) | bold}));
    //     }

    // private:
    //     monitor::metrics::CpuMonitor &monitor;
    // };

    class CpuWidget : public ftxui::ComponentBase
    {
        public:
            explicit CpuWidget(monitor::metrics::CpuMonitor &monitor)
                : monitor_(monitor) {}

            ftxui::Element Render() override
            {
                float cpu = monitor_.getCpuTotalUsage();

                push_sample(cpu);

                auto graph_fn = [this](int width, int height)
                {
                    std::vector<int> out(width, 0);

                    int n = (int)history_.size();
                    int start = n > width ? n - width : 0;

                    for (int x = 0; x < width; ++x)
                    {
                        int idx = start + x;
                        if (idx >= n)
                            break;
                        float v = history_[idx];
                        int h = (int)(v / 100.f * height); 
                        if (h < 0)
                            h = 0;
                        if (h > height)
                            h = height;
                        out[x] = h;
                    }
                    return out;
                };

                return ftxui::window(
                    ftxui::text("CPU"),
                    ftxui::vbox({
                        ftxui::text("Total: " + std::to_string((int)cpu) + "%") | ftxui::bold,
                        ftxui::separator(),
                        ftxui::graph(graph_fn) | ftxui::size(ftxui::HEIGHT, ftxui::GREATER_THAN, 6),
                    }));
            }

        private:
            monitor::metrics::CpuMonitor &monitor_;
            std::vector<float> history_;
            static constexpr std::size_t kMax = 300;

            void push_sample(float v)
            {
                history_.push_back(v);
                if (history_.size() > kMax)
                    history_.erase(history_.begin());
            }
    };

    class CpuWidget2 : public ftxui::ComponentBase {
        public:
            explicit CpuWidget2(monitor::metrics::CpuMonitor &monitor_) : monitor(monitor_) {}

            ftxui::Element Render() override {
                double total_usage = monitor.getCpuTotalUsage();
                double num_cores = monitor.getNumCores();
                cpu_usage_history.push_back(total_usage);

                for(int coreIdx = 0; coreIdx < num_cores; coreIdx++){
                    double core_usage = monitor.getCpuCoreUsage(coreIdx);
                    core_usage_history[coreIdx].push_back(core_usage);
                }

                auto graph_fn = [this](int width, int height)
                {
                    std::vector<int> out(width, 0);

                    int n = (int)cpu_usage_history.size();
                    int start = n > width ? n - width : 0;

                    for (int x = 0; x < width; ++x)
                    {
                        int idx = start + x;
                        if (idx >= n)
                            break;
                        double v = cpu_usage_history[idx];
                        int h = (int)(v / 100.f * height); 
                        if (h < 0)
                            h = 0;
                        if (h > height)
                            h = height;
                        out[x] = h;
                    }
                    return out;
                };

                return ftxui::window(
                    ftxui::text("CPU"),
                    ftxui::vbox({
                        ftxui::text("Total: " + std::to_string(total_usage) + "%") | ftxui::bold,
                        ftxui::separator(),
                        ftxui::graph(graph_fn) | ftxui::size(ftxui::HEIGHT, ftxui::GREATER_THAN, 6),
                    }));

            }

        private:
            monitor::metrics::CpuMonitor& monitor;
            std::vector<double> cpu_usage_history;
            std::vector<std::vector<double>> core_usage_history;
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

void thread_function(monitor::Engine& engine, ftxui::ScreenInteractive& screen){
    for(;;){
        engine.tick();
        screen.RequestAnimationFrame();

        std::cout << "HELLO " << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
};

int main()
{

    
    auto screen = ftxui::ScreenInteractive::Fullscreen();
    
    monitor::Engine engine;
    
    auto &cpuMonitor = engine.getCpuMonitor();
    auto cpuWidget = ftxui::Make<monitor::ui::CpuWidget>(cpuMonitor);
    
    std::thread engine_work(thread_function, std::ref(engine), std::ref(screen));

    screen.Loop(cpuWidget);

    return 0;
}