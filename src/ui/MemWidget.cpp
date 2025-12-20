
#include "monitor/ui/MemWidget.hpp"

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <cmath>

using namespace ftxui;

namespace monitor::ui
{
    // PUBLIC
    
    // Constructor
    MemWidget::MemWidget(MemMonitor &monitor_) : monitor(monitor_){}

    // Copy operator
    MemWidget::MemWidget(const MemWidget &widget)
        : ComponentBase(), monitor(widget.monitor)
    {}

    // Copy assignment operator
    MemWidget &MemWidget::operator=(const MemWidget &widget)
    {
        if (this == &widget)
            return *this;

        monitor = widget.monitor;

        return *this;
    }

    std::ostream &operator<<(std::ostream &os, const MemWidget &widget)
    {
        os << monitor::ansi::BOLD << monitor::ansi::GREEN << "MemWidget: " << monitor::ansi::RESET << std::endl;

        // Indent MemMonitor output
        std::ostringstream tempMon;
        tempMon << widget.monitor; // deref MemMonitor

        std::string line;
        std::istringstream streamMon(tempMon.str());
        while (std::getline(streamMon, line)) {
            os << "  " << line << std::endl;
        }

        return os;
    }

    Element MemWidget::OnRender()
    {
        const MemSnapshot& snap = monitor.getLatestSnapshot();
        Element mem_graph = RenderSection(
            "Physical Memory",
            snap.total,
            snap.used,
            snap.free,
            (snap.used / static_cast<double>(snap.total)) < 0.6 ? Color::GreenLight :
            ((snap.used / static_cast<double>(snap.total)) < 0.8 ? Color::YellowLight : Color::RedLight)
        );
        Element swap_graph = RenderSection(
            "Swap",
            snap.swapTotal,
            snap.swapUsed,
            snap.swapFree,
            (snap.swapUsed / static_cast<double>(snap.swapTotal)) < 0.6 ? Color::GreenLight :
            ((snap.swapUsed / static_cast<double>(snap.swapTotal)) < 0.8 ? Color::YellowLight : Color::RedLight)
        );

        return window(
            text("Memory Usage") | bold | ftxui::color(ftxui::Color::Cyan),
            vbox({
                mem_graph,
                separator(),
                swap_graph
            })
        ) | ftxui::color(ftxui::Color::Cyan);
    }


    // PRIVATE

    std::string toGB(uint64_t bytes) {
        double gib = bytes / 1024.0 / 1024.0 / 1024.0;
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2) << gib << " GB";
        return oss.str();
    }

    Element MemWidget::RenderBar(
        std::string label, uint64_t value, uint64_t total, Color color, uint64_t free) {
        double ratio = total ? double(value) / total : 0.0;
        return hbox({
            text(label) | bold | size(WIDTH, EQUAL, 12),
            gauge(ratio) | ftxui::color(color) | flex,
            separator(),
            text(toGB(value)) | size(WIDTH, EQUAL, 9),
            text(std::to_string(int(ratio * 100)) + "%") | size(WIDTH, EQUAL, 5),
            text(" Free: " + toGB(free)) | size(WIDTH, EQUAL, 15),
        });
    }

    Element MemWidget::RenderSection(
		const std::string& title,
        uint64_t total, uint64_t used, uint64_t free,
        Color bar_color
    ) {
        return vbox({
            text(title + ": " + toGB(total)) | bold,
            separator(),
            RenderBar("Used:", used, total, bar_color, free),
        });
    }
}