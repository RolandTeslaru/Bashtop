#include "monitor/ui/DialogManager.hpp"
#include <ftxui/component/component_base.hpp>
#include <string>

namespace monitor::ui {

    DialogManager& DialogManager::getInstance() {
        static DialogManager instance;
        return instance;
    }

    DialogManager::DialogManager() {
        ConfirmButton = ftxui::Button("OK", [&] { 
            show_dialog = false; 
        });

        using namespace ftxui;
        DialogComponent = Renderer(ConfirmButton, [&] {
            return window(
                text(dialog_title),
                vbox({
                    paragraph(dialog_body),
                    separator(),
                    ConfirmButton->Render() | center,
                })
            ) | clear_under | center | size(WIDTH, LESS_THAN, 50);
        });

        DialogComponent = CatchEvent(DialogComponent, [](const Event& e) {
            // consume arrow keys, rest goes to button
            if (e == Event::ArrowLeft || e == Event::ArrowRight ||
                e == Event::ArrowUp   || e == Event::ArrowDown) {
                return true;
            }
            return false; // propogate rest of events (Enter for button) to contents
        });

    }

    DialogManager::~DialogManager() = default;


    DialogManager& DialogManager::setDialogTitle(std::string title){
        this->dialog_title = std::move(title);
        return *this;
    }

    DialogManager& DialogManager::setDialogBody(std::string body){
        this->dialog_body = std::move(body);
        return *this;
    }

    DialogManager& DialogManager::show(){
        this->show_dialog = true;
        return *this;
    }

    DialogManager& DialogManager::hide(){
        this->show_dialog = false;
        return *this;
    }

    bool& DialogManager::isShowingDialog() {
        return this->show_dialog;
    }

    ftxui::Component DialogManager::getDialogComponent() {
        return this->DialogComponent;
    }
}