#include <string>
#include <vector>
#include <fstream>
#include "menu.h"
#include "tilepick.h"

enum settingType {BOOL, KEYBIND, DROPDOWN};

settingType type; // specifies the type of option being configured
int opt; // specifies the specific option being configured

// Vectors that store all settings changes prior to a save
std::vector<std::string> changesStrings; // e.g. "clear_messages"
std::vector<bool> changesBooleans; // e.g. Options.clear_messages

string _binaryToString(bool input) 
{
   return input ? "true" : "false";
}

string _getCurrentState(settingType settingType, int settingName)
{
    switch(settingType)
    {
        case BOOL:
            switch(settingName)
            {
                case 1:
                    return "Clear Messages \t\t\t" + _binaryToString(Options.clear_messages);
            }
    }
}

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
        add_entry(new CmdMenuEntry("Save Changes", MEL_ITEM, 's', CMD_SAVE_CHANGES));
        CmdMenuEntry* quitWOSavingME = new CmdMenuEntry("Back to Game Menu without saving", MEL_ITEM, 'Q', CMD_NO_CMD, false);
        quitWOSavingME->add_hotkey('q');
        add_entry(quitWOSavingME);

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
            bool _uses_popup = true, settingType _type = BOOL, int _option = 0)
            : MenuEntry(label, _level, 1, hotk), cmd(_cmd),
            uses_popup(_uses_popup), type(_type), option(_option)
        {
            if (tileidx_command(cmd) != TILEG_TODO)
                add_tile(tileidx_command(cmd));
        }

        command_type cmd;
        bool uses_popup;
        int option;
        settingType type;
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
        add_entry(new CmdMenuEntry(_getCurrentState(BOOL, 1), MEL_ITEM, '1', CMD_EDIT_OPTION, true, BOOL, 1));
        add_entry(new CmdMenuEntry("", MEL_SUBTITLE));
        add_entry(new CmdMenuEntry("Back to Settings Menu", MEL_ITEM, CK_ESCAPE,
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
    switch (type)
    {
        case BOOL: 
            switch (opt)
            {
                case 1:
                    // Change configuration
                    Options.clear_messages = !Options.clear_messages;

                    // Store which option was changed and what its new configuration is
                    std::string optionChanged = "clear_messages";
                    bool optionConfiguration = Options.clear_messages;

                    // Add configuration to changes vectors
                    bool configurationExists = false;
                    for (int i = 0; i < changesStrings.size(); i++)
                    {
                        if (changesStrings.at(i) == optionChanged)
                        {
                            configurationExists = true;
                            changesBooleans.at(i) = optionConfiguration;
                        }
                    }
                    if (!configurationExists)
                    {
                        changesStrings.push_back(optionChanged);
                        changesBooleans.push_back(optionConfiguration);
                    }

                    break;
            }
            break;
        case KEYBIND: 
            break;
        case DROPDOWN:
            break;
    }
}

// Save settings changes to init.txt
void saveChanges()
{
    // Open old init file
    std::fstream initFileOld;
    initFileOld.open("../settings/init.txt", ios::in);

    // Iterate through old init file and write to new init file
    std::string currentLine;
    std::vector<bool> configurationExistences(changesStrings.size(), false);
    if (initFileOld.is_open())
    {
        // Continue only if there are changes to be saved
        if (changesStrings.size() > 0)
        {
            // Open new init file
            std::fstream initFileNew;
            initFileNew.open("../settings/initNew.txt", fstream::trunc | fstream::out);

            // Modify configurations that already exist
            // Copy lines that do not contain relevant information
            while (std::getline(initFileOld, currentLine))
            {
                for (int i = 0; i < changesStrings.size(); i++)
                {
                    if (currentLine.find(changesStrings.at(i)) != string::npos)
                    {
                        configurationExistences.at(i) = true;

                        if (changesBooleans.at(i))
                            initFileNew << changesStrings.at(i) << " = true" << endl;
                        else
                            initFileNew << changesStrings.at(i) << " = false" << endl;
                    }
                    else
                        initFileNew << currentLine << endl;
                }
            }

            // Append configurations that do not already exist
            for (int i = 0; i < changesStrings.size(); i++)
            {
                if (!configurationExistences.at(i))
                {
                    if (changesBooleans.at(i))
                        initFileNew << changesStrings.at(i) << " = true" << endl;
                    else
                        initFileNew << changesStrings.at(i) << " = false" << endl;
                }
            }

            // Close new and old init files
            initFileOld.close();
            initFileNew.close();

            // Overwrite old init file with new init file
            std::remove("../settings/init.txt");
            std::rename("../settings/initNew.txt", "../settings/init.txt");

            // Output changes to message log
            for (int i = 0; i < changesStrings.size(); i++)
                mpr("Changes to " + changesStrings.at(i) + " saved to 'init.txt'.");

            // Clear changes vectors
            changesStrings.clear();
            changesBooleans.clear();
            configurationExistences.clear();
        }
    }
    else // init file unsuccessfully opened
    {
        mpr("Settings changes not saved since 'init.txt' could not be found.");
        mpr("Make sure 'init.txt' is located in the settings folder of the crawl-ref directory.");
    }
}
