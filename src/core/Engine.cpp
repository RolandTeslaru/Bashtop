#include <thread>
#include <sstream>

#include "monitor/metrics/CpuMonitor.hpp"
#include "monitor/metrics/MemMonitor.hpp"
#include "monitor/os/Factory.hpp"
#include "monitor/core/Engine.hpp"
#include <ftxui/component/screen_interactive.hpp>
#include <ostream>

#include "monitor/ansi.hpp"

namespace monitor {
    // Normal contrcutor, creates the readers and monitors itself
    Engine::Engine()
    :   cpuMonitor(std::make_shared<metrics::CpuMonitor>(os::make_cpu_reader())),
        memMonitor(std::make_shared<metrics::MemMonitor>(os::make_mem_reader()))
    {}



    // Constructor with custom readers
    Engine::Engine(
        std::unique_ptr<os::AbstractCpuReader> cpuReader,
        std::unique_ptr<os::AbstractMemReader> memReader
    )
    :   cpuMonitor(std::make_shared<metrics::CpuMonitor>(std::move(cpuReader))),
        memMonitor(std::make_shared<metrics::MemMonitor>(std::move(memReader)))
    {}



    // Copy constructor
    Engine::Engine(const Engine& other){
        this->cpuMonitor = other.cpuMonitor;
        this->memMonitor = other.memMonitor;
    }



    // Copy assignment operator
    Engine &Engine::operator=(const Engine &other){
        if(this == &other)
            return *this;

        this->cpuMonitor = other.cpuMonitor;
        this->memMonitor = other.memMonitor;

        return *this;
    }


    Engine::~Engine(){
        this->shutdown();
    }


    std::ostream& operator<<(std::ostream& os, const Engine& engine){
        os << monitor::ansi::BOLD << monitor::ansi::GREEN << "Engine: " << monitor::ansi::RESET << std::endl;

        // Indent CpuMonitor output
        std::ostringstream tempCpu;
        tempCpu << *(engine.cpuMonitor); // deref shared ptr

        std::string line;
        std::istringstream streamCpu(tempCpu.str());
        while (std::getline(streamCpu, line)) {
            os << "  " << line << std::endl;
        }

        // Indent MemMonitor output
        std::ostringstream tempMem;
        tempMem << *(engine.memMonitor); // deref shared ptr
        
        std::istringstream streamMem(tempMem.str());
        while (std::getline(streamMem, line)) {
            os << "  " << line << std::endl;
        }

        return os;
    }



    void Engine::tick(){
        this->cpuMonitor->computeSnapshot();
    }

    std::shared_ptr<monitor::metrics::CpuMonitor> Engine::getCpuMonitor(){
        return this->cpuMonitor;
    }

    [[maybe_unused]] std::shared_ptr<monitor::metrics::MemMonitor> Engine::getMemMonitor(){
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