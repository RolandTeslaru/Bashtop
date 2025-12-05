#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

#include "monitor/ui/MenubarWidget.hpp"

namespace monitor::ui
{
    MenubarWidget::MenubarWidget()
        : ComponentBase()
    {}

    MenubarWidget::MenubarWidget(const MenubarWidget &widget)
        : ComponentBase()
    {
        this->menuGroups     = widget.menuGroups;
        this->selectedGroup_ = widget.selectedGroup_;
        this->selectedItem_  = widget.selectedItem_;
        this->menuOpen_      = widget.menuOpen_;
    }

    MenubarWidget &MenubarWidget::operator=(const MenubarWidget &widget)
    {
        if (this == &widget)
            return *this; 

        this->menuGroups     = widget.menuGroups;
        this->menubar        = widget.menubar;
        this->menubarGroups  = widget.menubarGroups;
        this->selectedGroup_ = widget.selectedGroup_;
        this->selectedItem_  = widget.selectedItem_;
        this->menuOpen_      = widget.menuOpen_;
        
        return *this;
    }

    std::ostream &operator<<(std::ostream &os, const MenubarWidget &widget)
    {
        os << "MenubarWidget" << std::endl;
        os << "  Menu Groups: " << widget.menuGroups.size() << std::endl;
        for (const auto &group : widget.menuGroups) {
            os << "    Group: " << group.label << " (" << group.items.size() << " items)" << std::endl;
            for (const auto &item : group.items) {
                os << "      Item: " << item.label << std::endl;
            }
        }
        return os;
    }

    void MenubarWidget::registerMenuGroup(
        const std::string &label
    ) {
        MenuGroup newGroup;
        newGroup.label = label;
        menuGroups.push_back(newGroup);
    }

    void MenubarWidget::registerMenuItem(
        const std::string &groupLabel,
        const std::string &itemLabel,
        ftxui::Component itemComponent,
        std::function<void()> callback
    ) {
        for (auto &group : menuGroups) {
            if (group.label == groupLabel) {
                MenuItem newItem;
                newItem.label     = itemLabel;
                newItem.component = std::move(itemComponent);
                newItem.callback  = std::move(callback);
                group.items.push_back(std::move(newItem));
                return;
            }
        }
    }

    ftxui::Element MenubarWidget::OnRender() {
        using namespace ftxui;

        Elements items;
        for (int i = 0; i < (int)menuGroups.size(); ++i) {
            const auto &group = menuGroups[i];
            Element label = text(" " + group.label + " ");
            if (i == selectedGroup_)
                label = label | inverted;
            items.push_back(label);
        }
        menubarGroups = hbox(std::move(items));

        menubar = hbox({
            text(" Bashtop ") | bold,
            separator(),
            menubarGroups | flex,
        });

        return menubar;
    }

    ftxui::Element MenubarWidget::RenderOverlay() const {
        using namespace ftxui;

        if (!menuOpen_ || menuGroups.empty())
            return vbox({});

        if (selectedGroup_ < 0 || selectedGroup_ >= (int)menuGroups.size())
            return vbox({});

        const auto &group = menuGroups[selectedGroup_];

        if (group.items.empty())
            return vbox({});

        Elements item_elements;
        for (int i = 0; i < (int)group.items.size(); ++i) {
            Element line = text(" " + group.items[i].label + " ");
            if (i == selectedItem_)
                line = line | inverted;
            item_elements.push_back(line);
        }

        Element submenu_list = vbox(std::move(item_elements));

        Element submenu_window = window(
            text(group.label), submenu_list
        ) 
        | clear_under | size(HEIGHT, LESS_THAN, 6) | size(WIDTH, LESS_THAN, 24);

        Element submenu_overlay =
            vbox({
                hbox({
                    submenu_window,
                    filler(),
                }),
                filler(),
            });

        return submenu_overlay;
    }

    bool MenubarWidget::OnEvent(ftxui::Event event) {
        using namespace ftxui;
        if (menuGroups.empty())
            return false;

        if (menuOpen_) {
            const auto &group = menuGroups[selectedGroup_];

            if (event == Event::ArrowUp) {
                if (!group.items.empty()) {
                    selectedItem_ =
                        (selectedItem_ - 1 + (int)group.items.size()) %
                        (int)group.items.size();
                }
                return true;
            }

            if (event == Event::ArrowDown) {
                if (!group.items.empty()) {
                    selectedItem_ =
                        (selectedItem_ + 1) % (int)group.items.size();
                }
                return true;
            }

            if (event == Event::Escape) {
                menuOpen_ = false;
                return true;
            }

            if (event == Event::Return) {
                menuOpen_ = false;
                if (!group.items.empty() &&
                    selectedItem_ >= 0 &&
                    selectedItem_ < (int)group.items.size()) {
                    const auto &item = group.items[selectedItem_];
                    if (item.callback)
                        item.callback();
                }
                return true;
            }

            return true;
        }

        if (event == Event::ArrowLeft) {
            selectedGroup_ =
                (selectedGroup_ - 1 + (int)menuGroups.size()) %
                (int)menuGroups.size();
            return true;
        }

        if (event == Event::ArrowRight) {
            selectedGroup_ =
                (selectedGroup_ + 1) % (int)menuGroups.size();
            return true;
        }

        if (event == Event::Return) {
            if (!menuGroups[selectedGroup_].items.empty()) {
                menuOpen_     = true;
                selectedItem_ = 0;
                return true;
            }
        }

        return false;
    }
}