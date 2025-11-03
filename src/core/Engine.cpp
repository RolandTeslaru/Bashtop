#include <thread>

#include "monitor/metrics/CpuMonitor.hpp"
#include "monitor/metrics/MemMonitor.hpp"
#include "monitor/os/Factory.hpp"
#include "monitor/core/Engine.hpp"
#include <ftxui/component/screen_interactive.hpp>


namespace monitor {
    Engine::Engine()
    : cpuMonitor(monitor::os::make_cpu_reader()),
        memMonitor(monitor::os::make_mem_reader())
    {}

    Engine::Engine(
        std::unique_ptr<os::AbstractCpuReader> cpuReader,
        std::unique_ptr<os::AbstractMemReader> memReader
    )
    :   cpuMonitor(std::move(cpuReader)),
        memMonitor(std::move(memReader))
    {}

    void Engine::tick(){
        this->cpuMonitor.computeSnapshot();
    }

    monitor::metrics::CpuMonitor& Engine::getCpuMonitor(){
        return this->cpuMonitor;
    }

    [[maybe_unused]] monitor::metrics::MemMonitor& Engine::getMemMonitor(){
        return this->memMonitor;
    }

    

    void Engine::threadFunction(ftxui::ScreenInteractive &screen){
        while(this->isThreadRunning)
        {
            this->tick();
            screen.RequestAnimationFrame();

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    // Starts work
    void Engine::ignition(ftxui::ScreenInteractive &screen){
        if(this->isThreadRunning)
            return;

        this->isThreadRunning = true;
        
        this->engineThread = std::thread(&Engine::threadFunction, this, std::ref(screen));
    }

    void Engine::shutdown(){
        if(!this->isThreadRunning)
            return;

        this->isThreadRunning = false;

        if(this->engineThread.joinable())
            this->engineThread.join();
    }
}