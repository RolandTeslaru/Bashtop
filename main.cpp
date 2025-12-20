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
#include "monitor/metrics/PlatformInfoMonitor.hpp"
#include "monitor/os/Factory.hpp"
#include "monitor/core/Engine.hpp"

#include "monitor/ui/CpuWidget.hpp"
#include "monitor/ui/MemWidget.hpp"
#include "monitor/ui/PlatformInfoWidget.hpp"
#include "monitor/ui/MenubarWidget.hpp"

#include "monitor/ansi.hpp"
#include "monitor/exceptions/Platform.hpp"


using CpuMonitor    = monitor::metrics::CpuMonitor;
using vector_double = std::vector<double>;
using vector_int    = std::vector<int>;

using PlatformNotSupportedException = monitor::exceptions::PlatformNotSupportedException;

using CpuWidget          = monitor::ui::CpuWidget;
using MemWidget          = monitor::ui::MemWidget;
using PlatformInfoWidget = monitor::ui::PlatformInfoWidget;
using MenubarWidget      = monitor::ui::MenubarWidget;

using PlatformInfoMonitor = monitor::metrics::PlatformInfoMonitor;


void runTests(){
    std::cout << std::endl << monitor::ansi::BOLD << monitor::ansi::YELLOW <<"Running tests..." << monitor::ansi::RESET << std::endl << std::endl;
    
    
    auto platformInfo = PlatformInfoMonitor(
        monitor::os::make_platform_info_reader()
    );
    std::cout << platformInfo << std::endl;

    
    monitor::Engine engine;
    auto cpuMonitorPtr = engine.getCpuMonitor();
        
    std::cout << engine << std::endl;

    auto screen = ScreenInteractive::Fullscreen();

    auto cpuWidget = ftxui::Make<CpuWidget>(*cpuMonitorPtr);

    Component root = Renderer(cpuWidget, [cpuWidget] {
        return hbox({
            cpuWidget->Render() | ftxui::flex,
        }) | ftxui::flex;
    });

    std::cout << *cpuWidget << std::endl; // widget is a shared ptr so deref
}



int main()
{
    #if !defined(__linux__) && !defined(__APPLE__) && !defined(_WIN32)
        throw PlatformNotSupportedException(
            "This os is not supported yet."
        );
    #endif
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
    auto platformInfoMonitor = PlatformInfoMonitor(
        monitor::os::make_platform_info_reader()
    );

    auto screen = ScreenInteractive::Fullscreen();
    
    try {
        engine.ignition(screen);
    } catch (const std::exception& e) {
        std::cerr << "Failed to start engine: " << e.what() << std::endl;
        return 1;
    }

    auto cpuWidget          = ftxui::Make<CpuWidget>(*cpuMonitorPtr);
    auto memWidget          = ftxui::Make<MemWidget>(*memMonitorPtr);
    auto platformInfoWidget = ftxui::Make<PlatformInfoWidget>(platformInfoMonitor);

    Component WidgetsContentArea = Renderer(cpuWidget, [cpuWidget, memWidget, platformInfoWidget] {
        return hbox({
            vbox({
                memWidget->Render() | ftxui::flex,
                platformInfoWidget->Render() | ftxui::flex,
            }),
            cpuWidget->Render() | ftxui::flex,
        }) | ftxui::flex;
    });

    auto Menubar = ftxui::Make<MenubarWidget>();
    Menubar->registerMenuGroup("File");
    Menubar->registerMenuGroup("Help");
    Menubar->registerMenuGroup("Edit");
    Menubar->registerMenuGroup("View");

    bool show_dialog  = false;
    std::string dialog_title;
    std::string dialog_body;

    Menubar->registerMenuItem(
        "File",
        "About Bashtop",
        Component{}, // no compoennt currnetl
        [&] {
            show_dialog  = true;
            dialog_title = "About Bashtop";
            dialog_body  = "Bashtop is a terminal system monitor tool that tracks cpu and memory usage.\n";
        }
    );

    Menubar->registerMenuItem(
        "File",
        "Quit",
        Component{},
        [&] {
            screen.Exit();
        }
    );

    auto DialogButton = Button("OK", [&] { show_dialog = false; });


    Component StandardDialog = Renderer(DialogButton, [&] {
        return window(
            text(dialog_title),
            vbox({
                paragraph(dialog_body),
                separator(),
                DialogButton->Render() | center,
            })
        ) | clear_under | center | size(WIDTH, LESS_THAN, 50);
    });

    StandardDialog = CatchEvent(StandardDialog, [](Event e) {
        // consume arrow keys, rest goes to button
        if (e == Event::ArrowLeft || e == Event::ArrowRight ||
            e == Event::ArrowUp   || e == Event::ArrowDown) {
            return true;
        }
        return false; // propogate rest of events (Enter for button) to contents
    });

    Component layout = Container::Vertical({
        Menubar,
        WidgetsContentArea,
    });

    Component Layout = Renderer(layout, [&] {
        Element MenubarWidget = Menubar->Render();
        Element body          = WidgetsContentArea->Render() | flex;

        Element document = vbox({
            MenubarWidget,
            separator(),
            body,
        }) | flex;

        return dbox({
            document,
            Menubar->RenderOverlay(),
        });
    });

    Component root = Modal(Layout, StandardDialog, &show_dialog);

    root |= CatchEvent([Menubar, &show_dialog](Event event) {
        // catch events only if dialog is not shown
        if (show_dialog)
            return false;
        return Menubar->OnEvent(event);
    });

    screen.Loop(root);
    engine.shutdown();

    return 0;
}