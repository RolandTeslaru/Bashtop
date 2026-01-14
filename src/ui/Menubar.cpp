#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

#include "monitor/ui/Menubar.hpp"

namespace monitor::ui {
MenubarWidget::MenubarWidget() : ComponentBase() {}

MenubarWidget::MenubarWidget(const MenubarWidget &widget) : ComponentBase() {
  this->menuGroups      = widget.menuGroups;
  this->selectedGroup   = widget.selectedGroup;
  this->selectedItem    = widget.selectedItem;
  this->menuOpen        = widget.menuOpen;
}

MenubarWidget &MenubarWidget::operator=(const MenubarWidget &widget) {
  if (this == &widget)
    return *this;

  this->menuGroups      = widget.menuGroups;
  this->menubar         = widget.menubar;
  this->menuGroupsBar   = widget.menuGroupsBar;
  this->selectedGroup   = widget.selectedGroup;
  this->selectedItem    = widget.selectedItem;
  this->menuOpen        = widget.menuOpen;

  return *this;
}

std::ostream &operator<<(std::ostream &os, const MenubarWidget &widget) {
  os << "MenubarWidget" << std::endl;
  os << "  Menu Groups: " << widget.menuGroups.size() << std::endl;
  for (const auto &group : widget.menuGroups) {
    os << "    Group: " << group.label << " (" << group.items.size()
       << " items)" << std::endl;
    for (const auto &item : group.items) {
      os << "      Item: " << item.label << std::endl;
    }
  }
  return os;
}

ftxui::Element MenubarWidget::OnRender() {
  using namespace ftxui;

  Elements items;

  items.reserve(menuGroups.size());

  for (int i = 0; i < (int)menuGroups.size(); ++i) {
    Element label = text(" " + menuGroups[i].label + " ");
    if (i == selectedGroup)
      label = label | inverted;
    items.push_back(std::move(label));
  }
  auto groups_bar = hbox(std::move(items));

  return hbox({
      text(" Bashtop ") | bold,
      separator(),
      groups_bar | flex,
  });
}

ftxui::Element MenubarWidget::RenderOverlay() const {
  using namespace ftxui;

  // return blank if menu is not open
  if (!menuOpen || menuGroups.empty())
    return vbox({});

  if (selectedGroup < 0 || selectedGroup >= (int)menuGroups.size())
    return vbox({});

  const auto &group = menuGroups[selectedGroup];

  if (group.items.empty())
    return vbox({});

  // build group title and items
  Elements item_elements;
  for (int i = 0; i < (int)group.items.size(); ++i) {
    Element line = text(" " + group.items[i].label + " ");
    if (i == selectedItem)
      line = line | inverted;
    item_elements.push_back(line);
  }

  Element submenu_list = vbox(std::move(item_elements));

  Element submenu_window = window(text(group.label), submenu_list) |
                           clear_under | size(HEIGHT, LESS_THAN, 6) |
                           size(WIDTH, LESS_THAN, 24);

  Element submenu_overlay = vbox({
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

  if (menuOpen) {
    const auto &group = menuGroups[selectedGroup];

    if (event == Event::ArrowUp) {
      if (!group.items.empty()) {
        selectedItem = (selectedItem - 1 + (int)group.items.size()) %
                       (int)group.items.size();
      }
      return true;
    }

    if (event == Event::ArrowDown) {
      if (!group.items.empty()) {
        selectedItem = (selectedItem + 1) % (int)group.items.size();
      }
      return true;
    }

    if (event == Event::Escape) {
      menuOpen = false;
      return true;
    }

    if (event == Event::Return) {
      menuOpen = false;
      if (!group.items.empty() && selectedItem >= 0 &&
          selectedItem < (int)group.items.size()) {
        const auto &item = group.items[selectedItem];
        if (item.callback)
          item.callback();
      }
      return true;
    }

    return true;
  }

  if (event == Event::ArrowLeft) {
    selectedGroup =
        (selectedGroup - 1 + (int)menuGroups.size()) % (int)menuGroups.size();
    return true;
  }

  if (event == Event::ArrowRight) {
    selectedGroup = (selectedGroup + 1) % (int)menuGroups.size();
    return true;
  }

  if (event == Event::Return) {
    if (!menuGroups[selectedGroup].items.empty()) {
      menuOpen = true;
      selectedItem = 0;
      return true;
    }
  }

  return false;
}

MenubarBuilder &MenubarBuilder::addGroup(const std::string &label) {
  MenubarWidget::MenuGroup newGroup;
  newGroup.label = label;
  mw->menuGroups.push_back(newGroup);

  return *this;
}

MenubarBuilder &MenubarBuilder::addItem(const std::string &groupLabel,
                                        const std::string &itemLabel,
                                        ftxui::Component itemComponent,
                                        std::function<void()> callback) {
  for (auto &group : mw->menuGroups) {
    if (group.label == groupLabel) {
      MenubarWidget::MenuItem newItem;
      newItem.label = itemLabel;
      newItem.component = std::move(itemComponent);
      newItem.callback = std::move(callback);
      group.items.push_back(std::move(newItem));
      return *this;
    }
  }

  return *this;
}

std::shared_ptr<MenubarWidget> MenubarBuilder::build() { return mw; }
} // namespace monitor::ui
