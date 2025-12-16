
#include "monitor/ui/CpuWidget.hpp"

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <cmath>

#include "monitor/ansi.hpp"    

using namespace ftxui;


namespace monitor::ui
{
    // Constructor
    CpuWidget::CpuWidget(CpuMonitor &monitor_) : monitor(monitor_) {}

    // Copy constructor
    CpuWidget::CpuWidget(const CpuWidget &widget)
        : ComponentBase(), monitor(widget.monitor), cpu_usage_history(widget.cpu_usage_history)
    {}

    // Copy assignment operator
    CpuWidget &CpuWidget::operator=(const CpuWidget &widget)
    {
        if (this == &widget)
            return *this;

        cpu_usage_history = widget.cpu_usage_history;

        return *this;
    }

    // printing CpuWidget state
    std::ostream &operator<<(std::ostream &os, const CpuWidget &widget)
    {
        os << monitor::ansi::BOLD << monitor::ansi::GREEN << "CpuWidget: " << monitor::ansi::RESET << std::endl;
        os << "  CPU Usage History Size: " << widget.cpu_usage_history.size() << std::endl;
        os << "  Last Cpu Usage: "
           << (widget.cpu_usage_history.empty() ? 0.0 : widget.cpu_usage_history.back())
           << "%" << std::endl;

        // Indent CpuMonitor output
        std::ostringstream tempMon;
        tempMon << widget.monitor; // deref CpuMonitor

        std::string line;
        std::istringstream streamMon(tempMon.str());
        while (std::getline(streamMon, line)) {
            os << "  " << line << std::endl;
        }

        return os;
    }

    Element CpuWidget::OnRender()
    {
        double current_total_usage = monitor.getCpuTotalUsage();
        cpu_usage_history = monitor.getCpuUsageHistory();

        return window(
            text("CPU") | bold | ftxui::color(ftxui::Color::RedLight),
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

        ) | ftxui::color(ftxui::Color::RedLight);
    }

    std::vector<int> CpuWidget::graphTotalUsage(int width, int height) {
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
    }
}