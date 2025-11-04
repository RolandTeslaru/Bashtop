#include <iostream>
#include <chrono>
#include <thread>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
// #include <ftxui/component/>

#include <thread>

#include <cmath>
#include "monitor/metrics/CpuMonitor.hpp"
#include "monitor/metrics/SystemInfoProvider.hpp"
#include "monitor/os/Factory.hpp"
#include "monitor/core/Engine.hpp"
#include "monitor/ui/CpuWidget.hpp"
#include "monitor/ansi.hpp"

using namespace ftxui;

using CpuMonitor    = monitor::metrics::CpuMonitor;
using vector_double = std::vector<double>;
using vector_int    = std::vector<int>;


using namespace ftxui;

void runTests(){
    std::cout << std::endl << monitor::ansi::BOLD << monitor::ansi::YELLOW <<"Running tests..." << monitor::ansi::RESET << std::endl << std::endl;
    
    
    auto platformInfo = monitor::metrics::SystemInfoProvider(
        monitor::os::make_platform_info()
    );
    std::cout << platformInfo << std::endl;

    
    monitor::Engine engine;
    auto cpuMonitorPtr = engine.getCpuMonitor();
        
    std::cout << engine << std::endl;

    auto screen = ScreenInteractive::Fullscreen();


    auto cpuWidget = Make<monitor::ui::CpuWidget>(*cpuMonitorPtr);
    Component root = Renderer(cpuWidget, [cpuWidget] {
        return hbox({
            cpuWidget->Render() | ftxui::flex,
        }) | ftxui::flex;
    });

    std::cout << *cpuWidget << std::endl; // widget is a shared ptr so deref
}

int main()
{
    char a;
    std::cout << "Enter 0 to run tests, or anything else to start the monitor:" << std::endl;
    std::cin >> a;
    if (a == '0'){
        runTests();
        return 0;
    }

    
    monitor::Engine engine;
    auto cpuMonitorPtr = engine.getCpuMonitor();
        

    auto screen = ScreenInteractive::Fullscreen();
    
    engine.ignition(screen);


    auto cpuWidget = Make<monitor::ui::CpuWidget>(*cpuMonitorPtr);
    Component root = Renderer(cpuWidget, [cpuWidget] {
        return hbox({
            cpuWidget->Render() | ftxui::flex,
        }) | ftxui::flex;
    });

    screen.Loop(root);
    engine.shutdown();

    return 0;
}