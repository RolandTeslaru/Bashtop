#pragma once
#include <cstdint>
#include <memory>
#include <ostream>
#include "monitor/os/AbstractMemReader.hpp"
#include "monitor/types/RollingHistory.hpp"

using MemRawSample = monitor::types::mem::RawSample;
using MemSnapshot = monitor::types::mem::Snapshot;

namespace monitor::metrics {
    class MemMonitor {
        public:
            // ====================================================================
            // Constructor / Destructor
            // ====================================================================
            explicit MemMonitor(std::unique_ptr<monitor::os::AbstractMemReader> reader);
            MemMonitor(const MemMonitor& other);
            ~MemMonitor();

            // ====================================================================
            // Operator Overloads
            // ====================================================================
            friend std::ostream& operator<<(std::ostream& os, const MemMonitor& mon);

            MemMonitor& operator=(MemMonitor other);
            friend void swap(MemMonitor& Mon1, MemMonitor& Mon2) noexcept;

            // ====================================================================
            // Public Interface
            // ====================================================================
            void computeSnapshot();
            const MemSnapshot& getLatestSnapshot() const;

        private:
            // ====================================================================
            // Member Variables
            // ====================================================================
            static const std::size_t HISTORY_LIMIT = 240;

            std::unique_ptr<monitor::os::AbstractMemReader> memReader;

            bool hasSampledOnce = false;
            MemRawSample prevSample;
            MemSnapshot latestSnapshot;

            monitor::types::RollingHistory<std::uint64_t> used_history =
                monitor::types::RollingHistory<std::uint64_t>(HISTORY_LIMIT);
    };
}