

namespace monitor::ui {
    class MenubarWidget : public ftxui::ComponentBase {
        public:
            // Constructor / Destructor
            MenubarWidget();
            MenubarWidget(const MenubarWidget &widget);

            // Operator Overloads
            MenubarWidget &operator=(const MenubarWidget &widget);
            friend std::ostream& operator<<(std::ostream& os, const MenubarWidget& widget);

            // Public interface
            ftxui::Element OnRender() override;
            bool OnEvent(ftxui::Event event) override;

            // Render submenu overlay (goes above other widgets).
            ftxui::Element RenderOverlay() const;

            void registerMenuGroup(const std::string &label);

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

            // list of menu groups.
            std::vector<MenuGroup> menuGroups;

            // cached elements for rendering. 
            // TODO: add more elements and stuff
            ftxui::Element menubar;
            ftxui::Element menuGroupsBar;

            // State.
            int  selectedGroup = 0;
            int  selectedItem  = 0;
            bool menuOpen      = false;
    };
}