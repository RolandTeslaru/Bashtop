#pragma once

#ifdef __linux__

#include "monitor/os/AbstractMemReader.hpp"
#include "monitor/types/Mem.hpp"
#include <ostream>

namespace monitor::os::linux {

class MemReader final : public monitor::os::AbstractMemReader {
public:
	void sample(monitor::types::mem::RawSample& out) override;
	void print(std::ostream& os) const override;
};

}

#else
#error "MemReaderLinux.hpp should only be included on Linux (__linux__)"
#endif