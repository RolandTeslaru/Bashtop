#pragma once
#include <memory>
#include <ostream>
#include "monitor/os/AbstractMemReader.hpp"

using MemRawSample = monitor::types::mem::RawSample;
using MemSnapshot = monitor::types::mem::Snapshot;

namespace monitor::metrics {
    class MemMonitor {
        public:
            // ====================================================================
            // Constructor / Destructor
            // ====================================================================
            explicit MemMonitor(std::unique_ptr<monitor::os::AbstractMemReader> reader);
            ~MemMonitor();

            // ====================================================================
            // Operator Overloads
            // ====================================================================
            friend std::ostream& operator<<(std::ostream& os, const MemMonitor& mon);

            // ====================================================================
            // Public Interface
            // ====================================================================
            void computeSnapshot();
            const MemSnapshot& getLatestSnapshot() const;

        private:
            // ====================================================================
            // Member Variables
            // ====================================================================
            std::unique_ptr<monitor::os::AbstractMemReader> memReader;

            bool hasSampledOnce = false;
            MemRawSample prevSample;
            MemSnapshot latestSnapshot;
    };
}