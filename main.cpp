#include <iostream>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

using namespace ftxui;

int main()
{



    // Create a screen to render on.
    auto screen = ScreenInteractive::FitComponent();

    std::string message = "Waiting...";
    bool toggled = false;
    bool show_dialog = false;
    std::string input_text;

    auto on_hi = [&]
    {
        message = "BRUH BURH BRU";
        screen.Post(Event::Custom);
    };
    auto on_toggle = [&]
    {
        toggled = !toggled;
        screen.Post(Event::Custom);
    };
    auto on_open_dialog = [&]
  {
        show_dialog = true;
        screen.Post(Event::Custom);
    };
    auto on_quit = screen.ExitLoopClosure();

    auto btn_hi          = Button("Say BRUH",    on_hi);
    auto btn_toggle      = Button("Toggle",      on_toggle);
    auto btn_open_dialog = Button("Open Dialog", on_open_dialog);
    auto btn_quit        = Button("Quit",        on_quit, ButtonOption::Animated());

    auto buttons = Container::Horizontal({btn_hi, btn_toggle, btn_open_dialog, btn_quit});

    auto content = Renderer(buttons, [&]
        { return vbox({
            text("Header Bruh") | bold | color(Color::SlateBlue1),
            separator(),
            hbox({
                text("Status: "),
                text(message) | color(Color::Yellow),
            }),
            hbox({
                text("Toggled: "),
                text(toggled ? "ON" : "OFF") |
                    color(toggled ? Color::Green : Color::GrayDark),
            }),
            separator(),
            buttons->Render() | center,
        }) | border | center | size(WIDTH, GREATER_THAN, 40) | size(HEIGHT, GREATER_THAN, 10); });

    auto input = Input(&input_text, "Enter text...");
    auto dialog_close = Button("Close", [&]
        {
            show_dialog = false;
            if (!input_text.empty())
                message = "You typed: " + input_text;
            screen.Post(Event::Custom);
        });
    auto dialog_buttons = Container::Horizontal({dialog_close});
    auto dialog_container = Container::Vertical({input, dialog_buttons});

    auto dialog = Renderer(dialog_container, [&]
        {
            return vbox({
                        text("Dialog") | bold,
                        separator(),
                        input->Render(),
                        filler(),
                        dialog_buttons->Render() | hcenter,
                    }) |
                    border | center | size(WIDTH, GREATER_THAN, 40) |
                    size(HEIGHT, GREATER_THAN, 8) | bgcolor(Color::Black) |
                    color(Color::White);
        });

    auto app = Modal(content, dialog, &show_dialog);

    screen.Loop(app);
    return 0;
}

