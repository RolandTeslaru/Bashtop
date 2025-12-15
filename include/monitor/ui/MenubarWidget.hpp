

namespace monitor::ui {
    class MenubarWidget : public ftxui::ComponentBase {
        public:
            // ====================================================================
            // Constructor / Destructor
            // ====================================================================
            MenubarWidget();
            MenubarWidget(const MenubarWidget &widget);  // Copy constructor

            // ====================================================================
            // Operator Overloads
            // ====================================================================
            MenubarWidget &operator=(const MenubarWidget &widget);
            friend std::ostream& operator<<(std::ostream& os, const MenubarWidget& widget);

            // ====================================================================
            // Public Interface
            // ====================================================================
            ftxui::Element OnRender() override;
            bool OnEvent(ftxui::Event event) override;

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
            // ====================================================================
            // Member Variables
            // ====================================================================
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
            int selectedGroup_ = 0;
            int selectedItem_  = 0;
            bool menuOpen_     = false;
    };
}