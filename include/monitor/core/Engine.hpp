#include "monitor/metrics/CpuMonitor.hpp"
#include "monitor/metrics/MemMonitor.hpp"
#include "monitor/os/Factory.hpp"

class Engine {
    private:
        monitor::metrics::CpuMonitor cpuMonitor = monitor::metrics::CpuMonitor(
            monitor::os::make_cpu_reader()
        );
        
        monitor::metrics::MemMonitor memMonitor = monitor::metrics::MemMonitor(
            monitor::os::make_mem_reader()
        );

    public:
        Engine(){}
    
        void update(){
            this->cpuMonitor.computeSnapshot();
        }

        monitor::metrics::CpuMonitor& getCpuMonitor(){
            return this->cpuMonitor;
        }

        monitor::metrics::MemMonitor& getMemMonitor(){
            return this->memMonitor;
        }
};

namespace monitor {
    Engine engine = Engine();
}