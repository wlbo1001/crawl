#include <string>
#include <iostream>
#include <fstream>
#include "menu.h"
#include "tilepick.h"

int type; // specifies the type of option being configured
int opt; // specifies the specific option being configured

class EditOptionsMenu : public Menu
{
    // this could be easily generalized for other menus that select among commands
    // if it's ever needed
public:
    class CmdMenuEntry : public MenuEntry
    {
    public:
        CmdMenuEntry(string label, MenuEntryLevel _level, int hotk = 0,
            command_type _cmd = CMD_NO_CMD,
            bool _uses_popup = true)
            : MenuEntry(label, _level, 1, hotk), cmd(_cmd),
            uses_popup(_uses_popup)
        {
            if (tileidx_command(cmd) != TILEG_TODO)
                add_tile(tileidx_command(cmd));
        }

        command_type cmd;
        bool uses_popup;
    };

    command_type cmd;
    EditOptionsMenu()
        : Menu(MF_SINGLESELECT | MF_ALLOW_FORMATTING
            | MF_ARROWS_SELECT | MF_WRAP | MF_INIT_HOVER
#ifdef USE_TILE_LOCAL
            | MF_SPECIAL_MINUS // doll editor (why?)
#endif
        ),
        cmd(CMD_NO_CMD)
    {
        set_tag("game_menu");
        action_cycle = Menu::CYCLE_NONE;
        menu_action = Menu::ACT_EXECUTE;
        on_single_selection = [this](const MenuEntry& item)
        {
            const CmdMenuEntry* c = dynamic_cast<const CmdMenuEntry*>(&item);
            if (c)
            {
                if (c->uses_popup)
                {
                    // recurse
                    if (c->cmd != CMD_NO_CMD)
                        ::process_command(c->cmd, CMD_GAME_MENU);
                    return true;
                }
                // otherwise, exit menu and process in the main process_command call
                cmd = c->cmd;
                return false;
            }
            return true;
        };
    }

    bool skip_process_command(int keyin) override
    {
        if (keyin == '?')
            return true; // hotkeyed
        return Menu::skip_process_command(keyin);
    }

    void fill_entries()
    {
        clear();
        add_entry(new CmdMenuEntry("Edit Options Menu", MEL_TITLE));
        add_entry(new CmdMenuEntry("", MEL_SUBTITLE));
        // n.b. CMD_SAVE_GAME_NOW crashes on returning to the main menu if we
        // don't exit out of this popup now, not sure why
        add_entry(new CmdMenuEntry("Key Binds",//------------------------------------------------------------------------------------------------------
            MEL_ITEM, '*', CMD_EDIT_SUBOPTIONS));
        add_entry(new CmdMenuEntry("In Game Options",//------------------------------------------------------------------------------------------------------
            MEL_ITEM, '&', CMD_EDIT_SUBOPTIONS));
        add_entry(new CmdMenuEntry("", MEL_SUBTITLE));
        add_entry(new CmdMenuEntry("Back to game menu", MEL_ITEM, CK_ESCAPE,
            CMD_NO_CMD, false));
    }

    vector<MenuEntry*> show(bool reuse_selections = false) override
    {
        fill_entries();
        return Menu::show(reuse_selections);
    }
};

void openEditOptions()
{
    EditOptionsMenu m;
    m.show();
}



class EditSubOptionsMenu : public Menu
{
    // this could be easily generalized for other menus that select among commands
    // if it's ever needed
public:
    class CmdMenuEntry : public MenuEntry
    {
    public:
        CmdMenuEntry(string label, MenuEntryLevel _level, int hotk = 0,
            command_type _cmd = CMD_NO_CMD,
            bool _uses_popup = true, int _type = 0, int _option = 0)
            : MenuEntry(label, _level, 1, hotk), cmd(_cmd),
            uses_popup(_uses_popup), type(_type), option(_option)
        {
            if (tileidx_command(cmd) != TILEG_TODO)
                add_tile(tileidx_command(cmd));
        }

        command_type cmd;
        bool uses_popup;
        int option;
        int type;
    };

    command_type cmd;
    EditSubOptionsMenu()
        : Menu(MF_SINGLESELECT | MF_ALLOW_FORMATTING
            | MF_ARROWS_SELECT | MF_WRAP | MF_INIT_HOVER
#ifdef USE_TILE_LOCAL
            | MF_SPECIAL_MINUS // doll editor (why?)
#endif
        ),
        cmd(CMD_NO_CMD)
    {
        set_tag("game_menu");
        action_cycle = Menu::CYCLE_NONE;
        menu_action = Menu::ACT_EXECUTE;
        on_single_selection = [this](const MenuEntry& item)
        {
            const CmdMenuEntry* c = dynamic_cast<const CmdMenuEntry*>(&item);
            if (c)
            {
                type = c->type;
                opt = c->option;
                if (c->uses_popup)
                {
                    // recurse
                    if (c->cmd != CMD_NO_CMD)
                        ::process_command(c->cmd, CMD_GAME_MENU);
                    return true;
                }
                // otherwise, exit menu and process in the main process_command call
                cmd = c->cmd;
                return false;
            }
            return true;
        };
    }

    bool skip_process_command(int keyin) override
    {
        if (keyin == '?')
            return true; // hotkeyed
        return Menu::skip_process_command(keyin);
    }

    void fill_entries()
    {
        clear();
        add_entry(new CmdMenuEntry("Test Sub-Menu", MEL_TITLE));
        add_entry(new CmdMenuEntry("", MEL_SUBTITLE));
        items[1]->add_tile(tileidx_command(CMD_GAME_MENU));
        // n.b. CMD_SAVE_GAME_NOW crashes on returning to the main menu if we
        // don't exit out of this popup now, not sure why
        add_entry(new CmdMenuEntry("Clear Messages", MEL_ITEM, '1', CMD_EDIT_OPTION, false, 1, 0));
        for (int i = 1; i < 12; i++) {
            add_entry(new CmdMenuEntry("Test Option #" + std::to_string(i), MEL_ITEM));
        }
        add_entry(new CmdMenuEntry("", MEL_SUBTITLE));
        add_entry(new CmdMenuEntry("Back to game menu", MEL_ITEM, CK_ESCAPE,
            CMD_NO_CMD, false));
    }

    vector<MenuEntry*> show(bool reuse_selections = false) override
    {
        fill_entries();
        return Menu::show(reuse_selections);
    }
};

void openEditSubOptions()
{
    EditSubOptionsMenu m;
    m.show();
}

void changeSetting()
{
    mpr("changeSetting() called"); // debug output

    switch(type)
    {
        case 1: //bool
            switch(opt)
            {
                case 1:
                    // Change configuration
                    Options.clear_messages = !Options.clear_messages;

                    // Save configuration to init file
                    std::fstream initFileOld;
                    std::fstream initFileNew;
                    std::string currentLine;
                    std::string desiredOption = "clear_messages";
                    initFileOld.open("../settings/init.txt", ios::in);
                    initFileNew.open("../settings/initNew.txt", fstream::trunc | fstream::out);
                    if (initFileOld.is_open())
                    {
                        while (std::getline(initFileOld, currentLine))
                        {
                            if(currentLine.find(desiredOption) != string::npos)
                            {
                                if (Options.clear_messages)
                                {
                                    initFileNew << desiredOption << " = true" << endl;
                                }
                                else
                                {
                                    initFileNew << desiredOption << " = false" << endl;
                                }
                            }
                            else
                            {
                                initFileNew << currentLine << endl;
                            }
                        }
                    }
                    else
                    {
                        mpr("init.txt could not be found. Settings changes not saved.");
                    }
                    initFileOld.close();
                    initFileNew.close();
                    std::remove("../settings/init.txt");
                    std::rename("../settings/initNew.txt", "../settings/init.txt");

                    break;
            }
            break;
        case 2: //key bind
            break;
        case 3: //drop down
            break;
    }
}
