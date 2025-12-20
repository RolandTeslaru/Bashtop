#include "monitor/ui/PlatformInfoWidget.hpp"

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <cmath>

namespace monitor::ui 
{
    PlatformInfoWidget::PlatformInfoWidget(monitor::metrics::PlatformInfoMonitor& mon)
        : monitor(mon) 
    {}

    PlatformInfoWidget::PlatformInfoWidget(const PlatformInfoWidget &widget)
        : ComponentBase(), monitor(widget.monitor) 
    {}

    PlatformInfoWidget &PlatformInfoWidget::operator=(const PlatformInfoWidget &widget) 
    {
        if (this == &widget)
            return *this;

        // provider = widget.provider;

        return *this;
    }

    std::ostream &operator<<(std::ostream &os, const PlatformInfoWidget &widget) 
    {
        os << monitor::ansi::BOLD << monitor::ansi::GREEN << "PlatformInfoWidget: " << monitor::ansi::RESET << std::endl;

        // Indent PlatformInfoMonitor output
        std::ostringstream tempMonitor;
        tempMonitor << widget.monitor; // deref PlatformInfoMonitor

        std::string line;
        std::istringstream streamMonitor(tempMonitor.str());
        while (std::getline(streamMonitor, line)) {
            os << "  " << line << std::endl;
        }

        return os;
    }

    ftxui::Element PlatformInfoWidget::OnRender()
    {
        using namespace ftxui;

        const monitor::os::AbstractPlatformInfoReader& info = monitor.getPlatformInfo();

        Elements info_lines;

        info_lines.push_back(text("CPU Name:       " + info.getCpuName()));
        info_lines.push_back(text("Architecture:   " + info.getArch()));
        info_lines.push_back(text("Logical CPUs:   " + std::to_string(info.getLogicalCpus())));
        info_lines.push_back(text("Physical CPUs:  " + std::to_string(info.getPhysicalCpus())));
        info_lines.push_back(text("Total Memory:   " + std::to_string(info.getMemTotalBytes()) + " bytes"));
        info_lines.push_back(text("Page Size:      " + std::to_string(info.getPageSizeBytes()) + " bytes"));
        info_lines.push_back(text("OS Version:     " + info.getOsVersion()));
        info_lines.push_back(text("OS Build:       " + info.getOsBuild()));
        info_lines.push_back(text("Kernel Release: " + info.getKernelRelease()));
        info_lines.push_back(text("Model ID:       " + info.getModelId()));
        info_lines.push_back(text("Hostname:       " + info.getHostname()));

        return window(
            text(" Platform Info ") | bold | color(Color::GreenLight),
            vbox(std::move(info_lines))
        ) | color(Color::GreenLight);
    }
}