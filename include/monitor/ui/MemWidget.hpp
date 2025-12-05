#pragma once

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

#include "monitor/metrics/MemMonitor.hpp"
#include "monitor/metrics/SystemInfoProvider.hpp"
#include "monitor/os/Factory.hpp"
#include "monitor/core/Engine.hpp"

using MemMonitor    = monitor::metrics::MemMonitor;
using vector_double = std::vector<double>;
using vector_int    = std::vector<int>;

namespace monitor::ui {
    class MemWidget : public ftxui::ComponentBase {
        public:
            // ====================================================================
            // Constructor / Destructor
            // ====================================================================
            explicit MemWidget(MemMonitor& monitor_);
            MemWidget(const MemWidget &widget);  // Copy constructor

            // ====================================================================
            // Operator Overloads
            // ====================================================================
            MemWidget &operator=(const MemWidget& widget);
            friend std::ostream& operator<<(std::ostream& os, const MemWidget& widget);

            // ====================================================================
            // Public Interface
            // ====================================================================
            ftxui::Element OnRender() override;

        private:
            // ====================================================================
            // Private Helpers
            // ====================================================================
            ftxui::Element RenderSection(
                std::string title,
                uint64_t total, uint64_t used, uint64_t free,
                ftxui::Color bar_color
            );
            ftxui::Element RenderBar(
                std::string label,
                uint64_t value, uint64_t total,
                ftxui::Color color
            );

            // ====================================================================
            // Member Variables
            // ====================================================================
            MemMonitor& monitor;
    };
}