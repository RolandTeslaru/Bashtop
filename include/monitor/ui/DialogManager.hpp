#pragma once

#pragma once

#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <memory>
#include <string>

namespace monitor::ui {
class DialogManager {
public:
  static DialogManager &getInstance();
  DialogManager &setDialogTitle(std::string);
  DialogManager &setDialogBody(std::string);
  DialogManager &show();
  [[maybe_unused]] DialogManager &hide();
  ftxui::Component getDialogComponent();

  bool &isShowingDialog();

private:
  DialogManager();
  ~DialogManager();
  bool show_dialog = false;
  std::string dialog_title;
  std::string dialog_body;

  ftxui::Component ConfirmButton;
  ftxui::Component DialogComponent;
};
} // namespace monitor::ui