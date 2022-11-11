#include <string>
#include "menu.h"
#include "tilepick.h"


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
        add_entry(new CmdMenuEntry("Menu", MEL_TITLE));
        add_entry(new CmdMenuEntry("", MEL_SUBTITLE));
        add_entry(new CmdMenuEntry("Return to game", MEL_ITEM, CK_ESCAPE,
            CMD_NO_CMD, false));
        items[1]->add_tile(tileidx_command(CMD_GAME_MENU));
        // n.b. CMD_SAVE_GAME_NOW crashes on returning to the main menu if we
        // don't exit out of this popup now, not sure why
        add_entry(new CmdMenuEntry(
            (crawl_should_restart(game_exit::save)
                ? "Save and return to main menu"
                : "Save and exit"),
            MEL_ITEM, 'S', CMD_SAVE_GAME_NOW, false));
        add_entry(new CmdMenuEntry("It's a bomb!",//------------------------------------------------------------------------------------------------------
            MEL_ITEM, '*', CMD_EDIT_SUBOPTIONS));
        add_entry(new CmdMenuEntry("Help and manual",
            MEL_ITEM, '?', CMD_DISPLAY_COMMANDS));
        add_entry(new CmdMenuEntry("Lookup info",
            MEL_ITEM, '/', CMD_LOOKUP_HELP));
#ifdef TARGET_OS_MACOSX
        add_entry(new CmdMenuEntry("Show options file in finder",
            MEL_ITEM, 'O', CMD_REVEAL_OPTIONS));
#endif
        add_entry(new CmdMenuEntry("", MEL_SUBTITLE));
        add_entry(new CmdMenuEntry(
            "Quit and <lightred>abandon character</lightred>",
            MEL_ITEM, 'Q', CMD_QUIT, false));
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
        add_entry(new CmdMenuEntry("Submenu", MEL_TITLE));
        add_entry(new CmdMenuEntry("", MEL_SUBTITLE));
        add_entry(new CmdMenuEntry("Return to game", MEL_ITEM, CK_ESCAPE,
            CMD_NO_CMD, false));
        items[1]->add_tile(tileidx_command(CMD_GAME_MENU));
        // n.b. CMD_SAVE_GAME_NOW crashes on returning to the main menu if we
        // don't exit out of this popup now, not sure why
        add_entry(new CmdMenuEntry("It's a bomby!",//------------------------------------------------------------------------------------------------------
            MEL_ITEM));
        add_entry(new CmdMenuEntry("Help and manual",
            MEL_ITEM, '?', CMD_DISPLAY_COMMANDS));
        add_entry(new CmdMenuEntry("Lookup info",
            MEL_ITEM, '/', CMD_LOOKUP_HELP));
#ifdef TARGET_OS_MACOSX
        add_entry(new CmdMenuEntry("Show options file in finder",
            MEL_ITEM, 'O', CMD_REVEAL_OPTIONS));
#endif
        add_entry(new CmdMenuEntry("", MEL_SUBTITLE));
        add_entry(new CmdMenuEntry(
            "Quit and <lightred>abandon character</lightred>",
            MEL_ITEM, 'Q', CMD_QUIT, false));
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