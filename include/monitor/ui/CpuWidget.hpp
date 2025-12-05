#pragma once

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

#include "monitor/metrics/CpuMonitor.hpp"
#include "monitor/metrics/SystemInfoProvider.hpp"
#include "monitor/os/Factory.hpp"
#include "monitor/core/Engine.hpp"

using CpuMonitor    = monitor::metrics::CpuMonitor;
using vector_double = std::vector<double>;
using vector_int    = std::vector<int>;

using namespace ftxui;

namespace monitor::ui {
    class CpuWidget : public ftxui::ComponentBase {
        public:
            // ====================================================================
            // Constructor / Copy Constructor
            // ====================================================================
            explicit CpuWidget(CpuMonitor& monitor_);
            CpuWidget(const CpuWidget &widget);

            // ====================================================================
            // Operator Overloads
            // ====================================================================
            CpuWidget &operator=(const CpuWidget &widget);
            friend std::ostream &operator<<(std::ostream &os, const CpuWidget &widget);

            // ====================================================================
            // Public Interface
            // ====================================================================
            Element OnRender() override;

        private:
            // ====================================================================
            // Private Helpers
            // ====================================================================
            vector_int graphTotalUsage(int width, int height);

            // ====================================================================
            // Member Variables
            // ====================================================================
            CpuMonitor& monitor;
            vector_double cpu_usage_history;
    };
}