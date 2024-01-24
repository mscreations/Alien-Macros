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
#include <iostream>
#include <regex>

ProgSettings::ProgSettings(int argc, char* argv[])
{
    cxxopts::Options options("Alien-Macros", "Macro key translator");

    options.add_options()
        ("v,vid", "Target VID", cxxopts::value<short>())
        ("p,pid", "Target PID", cxxopts::value<short>())
        ("c,config", "Configuration file", cxxopts::value<std::string>()->default_value(DEFAULT_CFG_FILENAME))
        ;

    auto result = options.parse(argc, argv);

    std::string configFile = result["config"].as<std::string>();

    short vid{ 0 }, pid{ 0 };

    if (result.count("vid")) { vid = result["vid"].as<short>(); }
    if (result.count("pid")) { pid = result["pid"].as<short>(); }

    if (!Load(configFile))
    {
        std::cerr << "Configuration file could not be loaded" << std::endl;
        throw std::invalid_argument("Configuration file could not be loaded");
    }

    // if command line arguments for VID and PID are different than configuration, use them instead.
    if (vid != 0 && targetVID != vid) { targetVID = vid; }
    if (pid != 0 && targetPID != pid) { targetPID = pid; }
}

std::ostream& operator<<(std::ostream& strm, const ProgSettings& ps)
{
    strm << "CONFIGURATION:\n\nTargetDevice:\n";
    strm << std::format("VID: {:#06x} PID: {:#06x}\nUsagePage : {:#04x} Usage: {:#04x}\n\n",
                   ps.targetVID, ps.targetPID, ps.usagePage, ps.usageCode);

    strm << "# Macro Keys: " << ps.macrolist.size() << "\n\n";

    for (const auto& [key, macro] : ps.macrolist)
    {
        MacroAction action = static_cast<MacroAction>(macro);

        strm << std::format("Macro: {:s} ({:#04x})\n", action.getDescription(), key);
        switch (action.getActionCode())
        {
            case MacroActionCode::VirtualKey:
                strm << "Virtual Key Action\n";
                strm << std::format("    Payload: {:s}\n\n", GetKeyName(action.getVK()));
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

void ProgSettings::CreateBlank()
{
    // TODO: Add your implementation code here.
    targetPID = 0x0d62;
    targetVID = 0x1a1c;
}

int ProgSettings::getVID() const { return targetVID; }
int ProgSettings::getPID() const { return targetPID; }
int ProgSettings::getUsagePage() const { return usagePage; }
int ProgSettings::getUsageCode() const { return usageCode; }
std::string ProgSettings::getDescription(const short scancode) const
{
    return this->macrolist.at(scancode).getDescription();
}
std::unordered_map<short, MacroAction> ProgSettings::getMacros() const { return macrolist; }

bool ProgSettings::Save()
{
    return true;
}

bool ProgSettings::Load(const std::string filename)
{
    using namespace libconfig;

    configFilename = filename;

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
        Setting& target = configuration.lookup(TARGET_DEVICE);
        targetVID = target[VID];
        targetPID = target[PID];
        usagePage = target[USAGEPAGE];
        usageCode = target[USAGECODE];

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