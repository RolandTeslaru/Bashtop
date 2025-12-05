#pragma once

#include <string>
#include <vector>
#include <functional>
#include <ostream>

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

namespace monitor::ui
{
    class MenubarWidget : public ftxui::ComponentBase {
        public:
            explicit MenubarWidget();
            MenubarWidget(const MenubarWidget &widget); 

            MenubarWidget &operator=(const MenubarWidget &widget);
            friend std::ostream &operator<<(std::ostream &os, const MenubarWidget &widget);

            ftxui::Element OnRender() override;

            bool OnEvent(ftxui::Event event) override;

            ftxui::Element RenderOverlay() const;

            void registerMenuGroup(
                const std::string &label
            );
            
            void registerMenuItem(
                const std::string &groupLabel,
                const std::string &itemLabel,
                ftxui::Component itemComponent,
                std::function<void()> callback
            );
            

        private:
            struct MenuItem {
                std::string label;
                ftxui::Component component;
                std::function<void()> callback;
            };

            struct MenuGroup {
                std::string label;
                std::vector<MenuItem> items;
            };

            std::vector<MenuGroup> menuGroups;
            
            ftxui::Element menubar;
            ftxui::Element menubarGroups;

            
            int  selectedGroup_ = 0;
            int  selectedItem_  = 0;
            bool menuOpen_      = false;
    };
}