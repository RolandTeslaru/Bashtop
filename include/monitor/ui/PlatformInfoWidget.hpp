#pragma once

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

#include "monitor/metrics/CpuMonitor.hpp"
#include "monitor/metrics/PlatformInfoMonitor.hpp"
#include "monitor/os/Factory.hpp"
#include "monitor/core/Engine.hpp"

using namespace ftxui;

using PlatformInfoMonitor = monitor::metrics::PlatformInfoMonitor;

namespace monitor::ui {
    
    class PlatformInfoWidget : public ftxui::ComponentBase {

        public:
            // ====================================================================
            // Constructor / Copy Constructor
            // ====================================================================
            explicit PlatformInfoWidget(PlatformInfoMonitor& monitor);
            PlatformInfoWidget(const PlatformInfoWidget &widget);

            // ====================================================================
            // Operator Overloads
            // ====================================================================
            PlatformInfoWidget &operator=(const PlatformInfoWidget &widget);
            friend std::ostream &operator<<(std::ostream &os, const PlatformInfoWidget &widget);

            // ====================================================================
            // Public Interface
            // ====================================================================
            Element OnRender() override;

        private:
            // ====================================================================
            // Member Variables
            // ====================================================================
            PlatformInfoMonitor& monitor;
    };
}