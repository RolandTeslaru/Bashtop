#include "monitor/core/App.hpp"
#include "monitor/ui/CpuWidget.hpp"
#include "monitor/ui/DialogManager.hpp"
#include "monitor/ui/MemWidget.hpp"
#include "monitor/ui/PlatformInfoWidget.hpp"
#include "monitor/os/Factory.hpp"
#include <string>

#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <iostream>
#include <chrono>
#include <cstdlib>
#include <thread>

const int MIN_WIDTH = 120;
const int MIN_HEIGHT = 30;

using CpuWidget = monitor::ui::CpuWidget;
using MemWidget = monitor::ui::MemWidget;
using PlatformInfoWidget = monitor::ui::PlatformInfoWidget;
using MenubarWidget = monitor::ui::MenubarWidget;
using DialogManager = monitor::ui::DialogManager;
using PlatformInfoMonitor = monitor::metrics::PlatformInfoMonitor;

namespace monitor
{
    App::App() : platformInfoMonitor(monitor::os::make_platform_info_reader())
    {
        using namespace ftxui;

        auto cpuMonitorPtr = engine.getCpuMonitor();
        auto memMonitorPtr = engine.getMemMonitor();
        auto cpuWidget = ftxui::Make<CpuWidget>(*cpuMonitorPtr);
        auto memWidget = ftxui::Make<MemWidget>(*memMonitorPtr);
        auto platformInfoWidget =
            ftxui::Make<PlatformInfoWidget>(this->platformInfoMonitor);

        WidgetsContentArea =
            Renderer(cpuWidget,
                     [cpuWidget, memWidget, platformInfoWidget]
                     {
                         return hbox({
                                    vbox({
                                        memWidget->Render() | ftxui::flex,
                                        platformInfoWidget->Render() | ftxui::flex,
                                    }) | ftxui::xflex_grow,
                                    cpuWidget->Render() | ftxui::flex,
                                }) |
                                ftxui::xflex_shrink;
                     });

        auto menubarBuilder = monitor::ui::MenubarBuilder();

        Menubar = menubarBuilder.addGroup("File")
            .addGroup("Help")
            .addGroup("Edit")
            .addGroup("Viewo")
            .addItem("File", "About Bashtop",
                Component{}, // no compoennt currnetl
                []{
                    monitor::ui::DialogManager::getInstance()
                        .setDialogTitle("About Bashtop")
                        .setDialogBody("Bashtop is a terminal system monitor tool that " "tracks cpu and memory usage.\n")
                        .show();
                })
            .addItem("File", "Quit", Component{}, [this]
                        { this->screen.Exit(); })
            .build();

        Component layout = Container::Vertical({
            Menubar,
            WidgetsContentArea,
        });

        auto Layout = Renderer(layout, 
            [this]{
                auto dimensions = ftxui::Terminal::Size();
                if (dimensions.dimx < MIN_WIDTH || dimensions.dimy < MIN_HEIGHT) {
                    return vbox({
                        filler(),
                        vbox({
                            text("Please resize the terminal window to at least " +
                                std::to_string(MIN_WIDTH) + "x" +
                                std::to_string(MIN_HEIGHT)) |
                                hcenter | color(Color::GreenLight),
                            separator(),
                            text("Current size is " + std::to_string(dimensions.dimx) +
                                "x" + std::to_string(dimensions.dimy)) |
                                hcenter | color(Color::Red),
                        }) | hcenter,
                        filler(),
                    }) | flex;
                }

                Element menubar_element = Menubar->Render();
                Element body = WidgetsContentArea->Render() | flex;

                Element document = vbox({
                    menubar_element, separator(), body,
                }) | flex;

                return dbox({
                    document,
                    Menubar->RenderOverlay(),
                }); 
            });

            root = Modal(Layout, monitor::ui::DialogManager::getInstance().getDialogComponent(),
                        &monitor::ui::DialogManager::getInstance().isShowingDialog());

            root = ftxui::CatchEvent(root, 
                [this](Event event){
                    // catch events only if dialog is not shown
                    if (monitor::ui::DialogManager::getInstance().isShowingDialog())
                        return false;

                    return Menubar->OnEvent(event); 
                }
        );
    }

    App &App::getInstance()
    {
        static App instance;
        return instance;
    }

    Engine &App::getEngine()
    {
        return App::getInstance().engine;
    }

    void App::startFrameLoop()
    {
        try{
            this->engine.ignition(this->screen);
        }
        catch (const std::exception &e){
            std::cerr << "Failed to start engine: " << e.what() << std::endl;
            return;
        }

        if (const char *auto_exit_ms = std::getenv("BASHTOP_AUTO_EXIT_MS"))
        {
            const int ms = std::max(0, std::atoi(auto_exit_ms));
            std::thread([this, ms]
                        {
                std::this_thread::sleep_for(std::chrono::milliseconds(ms));
                this->screen.Exit(); })
                .detach();
        }

        this->screen.Loop(this->root);
    }

    void App::shutdown()
    {
        this->engine.shutdown();
    }
}