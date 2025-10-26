#include <vector>
#include <cstdint>
#include <chrono>

#include "monitor/os/AbstractCpuReader.hpp"
#include "monitor/types/Cpu.hpp"

// To read the CPU usage on Mac systems,
// we can use the Mach host API to get CPU load information.
#include <mach/mach_host.h>
#include <mach/processor_info.h>
#include <mach/mach.h>

using CpuRawSample = monitor::types::cpu::RawSample;
using CpuCoreTicks = monitor::types::cpu::CoreTicks;

using Clock = std::chrono::steady_clock;
using Nanoseconds = std::chrono::nanoseconds;

namespace monitor::os::mac
{
    class CpuReader final : public monitor::os::AbstractCpuReader{
        public:
            bool sample(CpuRawSample &out) override{

                natural_t cpuCount = 0;
                processor_info_array_t info = nullptr; // this type is a generic pointer type so it allows us to return difrent kinds of data dpeending on the flavor
                mach_msg_type_number_t infoCount = 0;

                const kern_return_t kr = host_processor_info(
                    mach_host_self(),
                    PROCESSOR_CPU_LOAD_INFO,
                    &cpuCount,
                    &info,
                    &infoCount);

                if (kr != KERN_SUCCESS || info == nullptr || cpuCount == 0)
                    return false;

                const auto now = Clock::now().time_since_epoch();

                out.timestamp_ns = this->to_nanoseconds(now);

                out.per_core.clear();
                out.per_core.resize(cpuCount);

                this->read_loads(info, cpuCount, out);

                vm_deallocate(
                    mach_task_self(),
                    reinterpret_cast<vm_address_t>(info),
                    infoCount * sizeof(integer_t));

                return true;
            }

    private:
        static uint64_t to_nanoseconds(const Clock::duration &duration){
            return static_cast<uint64_t>(
                std::chrono::duration_cast<Nanoseconds>(duration).count());
        }

        static void read_loads(
            processor_info_array_t info,
            natural_t cpuCount,
            CpuRawSample &out)
        {
            uint64_t totalIdle = 0;
            uint64_t totalAll = 0;

            auto loads = reinterpret_cast<processor_cpu_load_info_t>(info);

            for (natural_t coreIdx = 0; coreIdx < cpuCount; ++coreIdx)
            {

                const uint64_t user = loads[coreIdx].cpu_ticks[CPU_STATE_USER];
                const uint64_t sys  = loads[coreIdx].cpu_ticks[CPU_STATE_SYSTEM];
                const uint64_t idle = loads[coreIdx].cpu_ticks[CPU_STATE_IDLE];
                const uint64_t nice = loads[coreIdx].cpu_ticks[CPU_STATE_NICE];

                const uint64_t all = user + sys + idle + nice;

                out.per_core[coreIdx].idle = idle;
                out.per_core[coreIdx].total = all;

                totalIdle += idle;
                totalAll += all;
            }

            out.total.idle = totalIdle;
            out.total.total = totalAll;
        }
    };
};



namespace monitor::os {
    std::unique_ptr<AbstractCpuReader> make_cpu_reader(){
        return std::make_unique<mac::CpuReader>();
    }
}