#include <string>
#include "menu.h"
#include "tilepick.h"

class EditOptionsMenu : public Menu
{
    public:
    EditOptionsMenu(command_type where_from=CMD_NO_CMD) : Menu(MF_SINGLESELECT | MF_ALLOW_FORMATTING | MF_ARROWS_SELECT | MF_WRAP), back_cmd(where_from)
    {
        action_cycle = Menu::CYCLE_NONE;
        menu_action  = Menu::ACT_EXECUTE;
        set_title(new MenuEntry("Edit Settings Menu:", MEL_TITLE));
    }

    void fill_entries()
    {
        clear();
        const string back = back_cmd == CMD_GAME_MENU
                                ? "Back to game menu"
                                : "Exit help lookup";
        auto back_button = new MenuEntry(back, MEL_ITEM, 1, CK_ESCAPE);//back = text on screen, MEL_ITEM = enum type of Item, 1 = amount, CK_ESCAPE = also selectable by pressing ESC
        if (back_cmd != CMD_NO_CMD)
            back_button->add_tile(tileidx_command(back_cmd));

        auto submenu_button = new MenuEntry("goto Submenu", MEL_ITEM);
        submenu_button->add_tile(tileidx_command(CMD_EDIT_SUBOPTIONS));
        add_entry(submenu_button);


        for (int i = 0; i < 12; i++)
        {
            add_entry(new MenuEntry("Test Option #" + std::to_string(i), MEL_ITEM));
        }
        add_entry(new MenuEntry("", MEL_SUBTITLE));
        add_entry(back_button);
    }

    vector<MenuEntry *> show(bool reuse_selections = false)
    {
        fill_entries();
        cycle_hover();
        return Menu::show(reuse_selections);
    }
    
    private:
    command_type back_cmd;
};

void openEditOptions(command_type where_from)
{
    EditOptionsMenu m(where_from);
    m.show();
}



class EditSubOptionsMenu : public Menu {
    public:
        EditSubOptionsMenu(command_type where_from = CMD_NO_CMD) : Menu(MF_SINGLESELECT | MF_ALLOW_FORMATTING | MF_ARROWS_SELECT | MF_WRAP), back_cmd(where_from) {
            action_cycle = Menu::CYCLE_NONE;
            menu_action = Menu::ACT_EXECUTE;
            set_title(new MenuEntry("Edit Subsetting Menu:", MEL_TITLE));
        }

        void fill_entry() {
            clear();
            add_entry(new MenuEntry("TEST_ENTRY"));

            const string back = back_cmd == CMD_GAME_MENU
                ? "Back to game menu"
                : "Exit help lookup";
            auto back_button = new MenuEntry(back, MEL_ITEM, 1, CK_ESCAPE);//back = text on screen, MEL_ITEM = enum type of Item, 1 = amount, CK_ESCAPE = also selectable by pressing ESC
            if (back_cmd != CMD_NO_CMD)
                back_button->add_tile(tileidx_command(back_cmd));
            add_entry(back_button);
        }

        vector<MenuEntry*> show(bool reuse_selections = false)
        {
            fill_entry();
            cycle_hover();
            return Menu::show(reuse_selections);
        }

    private:
        command_type back_cmd;
            
};

void openEditSubOptions(command_type where_from) {
    EditSubOptionsMenu m(where_from);
    m.show();
}