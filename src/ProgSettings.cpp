/*
 *  Alien-Macros
 *  Receives Macro keypresses and translates to a keycode that AutoHotKey can understand.
 *  Copyright (C) 2023 mscreations
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

#include "ProgSettings.h"
#include <format>
#include <iomanip>
#include <fstream>
#include <iostream>
#include <regex>
#include <limits>
#include "colors.h"
#include "HidDevice.h"
#include "setup.h"

ProgSettings::ProgSettings() : target{ 0,0,0,0 }, configFilename{} {}

ProgSettings::ProgSettings(int argc, const char* argv[])
{
    std::string configFile{};

    for (int i = 1; i < argc; i++)
    {
        std::string value = argv[i];
        if (value.ends_with(".cfg"))
        {
            configFile = value;
            break;
        }
    }

    if (configFile == "") { configFile = DEFAULT_CFG_FILENAME; }

    std::ifstream file(configFile);
    if (file)
    {
        if (!file.is_open())
        {
            std::cerr << "Config file exists, but is not readable." << std::endl;
            exit(false);
        }
        file.close();
    }
    else
    {
        std::cerr << "Config file does not exists." << std::endl;

        if (Utils::AskResponse("Do you want to run the setup function?", /*yesDefault = */ true))
        {
            auto newSetup = Setup::invokeSetup();
            target = newSetup->target;
            if (!newSetup->macrolist.empty())
            {
                macrolist = newSetup->macrolist;
            }
            return; // TODO this should save the new configuration now that we have it created and in theory tested it.
        }
        else
        {
            std::cerr << "No valid configuration. Exiting." << std::endl;
            exit(false);
        }
    }

    if (!Load(configFile))
    {
        std::cerr << "Configuration file could not be loaded" << std::endl;
        throw std::invalid_argument("Configuration file could not be loaded");
    }
}

ProgSettings::ProgSettings(ProgSettings& ps)
{
    target = ps.target;

    if (!ps.macrolist.empty())
    {
        macrolist = ps.macrolist;
    }
}

std::ostream& operator<<(std::ostream& strm, const ProgSettings& ps)
{
    strm << Colors::Red << "CONFIGURATION:" << Colors::Reset << "\n\nTargetDevice:\n";
    strm << std::format("VID: {:#06x} PID: {:#06x}\nUsagePage : {:#04x} Usage: {:#04x}\n\n",
                   ps.target.targetVID, ps.target.targetPID, ps.target.usagePage, ps.target.usageCode);

    strm << "# Macro Keys: " << ps.macrolist.size() << "\n\n";

    for (const auto& [key, macro] : ps.macrolist)
    {
        MacroAction action = static_cast<MacroAction>(macro);

        strm << std::format("Macro: {:s} ({:#04x})\n", action.getDescription(), key);
        switch (action.getActionCode())
        {
            case MacroActionCode::VirtualKey:
                strm << "Virtual Key Action\n";
                strm << std::format("    Payload: {:s}\n\n", Utils::GetKeyName(action.getVK()));
                break;
            case MacroActionCode::Char:
                strm << "Char Action\n";
                strm << std::format("    Payload: {}\n\n", action.getChar());
                break;
            case MacroActionCode::String:
                strm << "String Action\n";
                strm << std::format("    Payload: {:s}\n\n", action.getString());
                break;
            default:
                strm << "Unknown/Undefined action code\n\n";
                break;
        }
    }
    return strm;
}

unsigned short ProgSettings::getVID() const { return target.targetVID; }
unsigned short ProgSettings::getPID() const { return target.targetPID; }
unsigned short ProgSettings::getUsagePage() const { return target.usagePage; }
unsigned short ProgSettings::getUsageCode() const { return target.usageCode; }

/// <summary>
/// Retrieves the description of the specified macro scancode. Returns generic message if macro doesn't exist.
/// </summary>
/// <param name="scancode">Macro scan code to lookup</param>
/// <returns>std::string Description or generic response</returns>
std::string ProgSettings::getDescription(const short scancode) const
{
    if (macrolist.find(scancode) == macrolist.end())
    {
        return "Macro missing";
    }
    return this->macrolist.at(scancode).getDescription();
}

std::unordered_map<short, MacroAction> ProgSettings::getMacros() const { return macrolist; }

bool ProgSettings::Load(const std::string& filename)
{
    using namespace libconfig;

    configFilename = filename;

    libconfig::Config configuration;

    // Load and parse the file with libconfig
    try
    {
        configuration.readFile(filename);
    }
    catch (const FileIOException& fioex)
    {
        std::cerr << "IO error reading file" << std::endl;
        std::cerr << fioex.what() << std::endl;
        return false;
    }
    catch (const ParseException& pex)
    {
        std::cerr << std::format("Parse error at {}:{} - {}", pex.getFile(), pex.getLine(), pex.getError()) << std::endl;
        return false;
    }

    try
    {
        // Extract targetdevice details from configuration
        Setting& trgt = configuration.lookup(TARGET_DEVICE);
        target = { trgt[VID], trgt[PID], trgt[USAGEPAGE], trgt[USAGECODE] };

        // Extract macro information from configuration
        Setting& macros = configuration.lookup(MACROS);

        for (Setting& macro : macros)
        {
            int scancode{ 0 }, payloadtype{ 0 }, actioncode{ 0 };
            short shortscancode{ 0 };
            std::string description{};

            macro.lookupValue(SCANCODE, scancode);
            macro.lookupValue(PAYLOAD_TYPE, payloadtype);
            macro.lookupValue(MACRO_ACTION, actioncode);
            macro.lookupValue(MACRO_KEY_DESCRIPTION, description);

            shortscancode = static_cast<short>(scancode);

            if (payloadtype == Setting::TypeString)
            {
                std::string stringPayload{};
                macro.lookupValue(MACRO_PAYLOAD, stringPayload);
                macrolist[shortscancode] = MacroAction(stringPayload, description);
            }
            else if (payloadtype == Setting::TypeInt)
            {
                int intPayload{ 0 };
                macro.lookupValue(MACRO_PAYLOAD, intPayload);

                switch (static_cast<MacroActionCode>(actioncode))
                {
                    case MacroActionCode::VirtualKey:
                        macrolist[shortscancode] = MacroAction(static_cast<short>(intPayload), description);
                        break;
                    case MacroActionCode::Char:
                        macrolist[shortscancode] = MacroAction(static_cast<char>(intPayload), description);
                        break;
                }
            }
        }
    }
    catch (const SettingNotFoundException& snfe)
    {
        std::cerr << std::format("{} - Could not load configuration information:\nMissing Path: {}", snfe.what(), snfe.getPath()) << std::endl;
        return false;
    }
    catch (const SettingTypeException& ste)
    {
        std::cerr << std::format("{} - Invalid Type\nPath: {}", ste.what(), ste.getPath()) << std::endl;
        return false;
    }

    return true;
}
