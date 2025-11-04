#pragma once

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

#include "monitor/metrics/CpuMonitor.hpp"
#include "monitor/metrics/SystemInfoProvider.hpp"
#include "monitor/os/Factory.hpp"
#include "monitor/core/Engine.hpp"

using CpuMonitor = monitor::metrics::CpuMonitor;
using vector_double = std::vector<double>;
using vector_int = std::vector<int>;

using namespace ftxui;

namespace monitor::ui
{
    class CpuWidget : public ftxui::ComponentBase
    {
    public:
        explicit CpuWidget(CpuMonitor& monitor_);

        CpuWidget &operator=(const CpuWidget &widget);
        CpuWidget(const CpuWidget &widget);

        friend std::ostream &operator<<(std::ostream &os, const CpuWidget &widget);

        Element OnRender() override
        {
            double current_total_usage = monitor.getCpuTotalUsage();
            cpu_usage_history = monitor.getCpuUsageHistory();

            return window(
                text("CPU"),
                hbox({
                    vbox({
                        text("Total: " + std::to_string(current_total_usage) + "%") | bold,
                        separator(),
                        graph(
                            [this](int width, int height) { return this->graphTotalUsage(width, height); }
                        ) | color(Color::RedLight),
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

        vector_int graphTotalUsage(int width, int height);

        CpuMonitor &monitor;
        vector_double cpu_usage_history;
    };
}