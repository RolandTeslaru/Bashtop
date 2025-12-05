#include <iostream>
#include <chrono>
#include <thread>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
// #include <ftxui/component/>

#include <thread>
#include <cmath>
#include <vector>
#include <string>

#include "monitor/metrics/CpuMonitor.hpp"
#include "monitor/metrics/SystemInfoProvider.hpp"
#include "monitor/os/Factory.hpp"
#include "monitor/core/Engine.hpp"

#include "monitor/ui/CpuWidget.hpp"
#include "monitor/ui/MemWidget.hpp"
#include "monitor/ui/MenubarWidget.hpp"

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
    auto memMonitorPtr = engine.getMemMonitor();

    auto screen = ScreenInteractive::Fullscreen();
    
    engine.ignition(screen);

    auto cpuWidget = Make<monitor::ui::CpuWidget>(*cpuMonitorPtr);
    auto memWidget = Make<monitor::ui::MemWidget>(*memMonitorPtr);

    Component content = Renderer(cpuWidget, [cpuWidget, memWidget] {
        return hbox({
            memWidget->Render() | ftxui::flex,
            cpuWidget->Render() | ftxui::flex,
        }) | ftxui::flex;
    });

    auto menubarWidget = Make<monitor::ui::MenubarWidget>();
    menubarWidget->registerMenuGroup("File");
    menubarWidget->registerMenuGroup("Help");
    menubarWidget->registerMenuGroup("Edit");
    menubarWidget->registerMenuGroup("View");

    bool show_dialog  = false;
    std::string dialog_title;
    std::string dialog_body;

    using namespace ftxui;

    menubarWidget->registerMenuItem(
        "File",
        "About Bashtop",
        Component{}, // no compoennt currnetl
        [&] {
            show_dialog  = true;
            dialog_title = "About Bashtop";
            dialog_body  = "Bashtop system monitor\n"
                           "Powered by FTXUI.\n\n"
                           "Use OK to close.";
        }
    );

    menubarWidget->registerMenuItem(
        "File",
        "Quit",
        Component{},
        [&] {
            screen.Exit();
        }
    );

    auto DialogButton = Button("OK", [&] { show_dialog = false; });

    Component Dialog = Renderer(DialogButton, [&] {
        return window(
            text(dialog_title),
            vbox({
                paragraph(dialog_body),
                separator(),
                DialogButton->Render() | center,
            })
        ) | clear_under | center | size(WIDTH, LESS_THAN, 50);
    });

    Component layout = Container::Vertical({
        menubarWidget,
        content,
    });

    Component Layout = Renderer(layout, [&] {
        Element MenubarWidget = menubarWidget->Render();
        Element body          = content->Render() | flex;

        Element document = vbox({
            MenubarWidget,
            separator(),
            body,
        }) | flex;

        return dbox({
            document,
            menubarWidget->RenderOverlay(),
        });
    });

    Component root = Modal(Layout, Dialog, &show_dialog);

    root |= CatchEvent([&, menubarWidget](Event event) {
        return menubarWidget->OnEvent(event);
    });

    screen.Loop(root);
    engine.shutdown();

    return 0;
}