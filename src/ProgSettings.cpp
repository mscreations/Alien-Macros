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
#include <sstream>
#include <iostream>
#include <regex>
#include <limits>
#include "colors.h"
#include "HidDevice.h"
#include "setup.h"
#include "RegistryHelper.h"

ProgSettings::ProgSettings() : target{ 0,0,0,0 }, configFilename{}
{
    if (!Load())
    {
        std::cerr << "Unable to load configuration from Registry" << std::endl;
    }
}

ProgSettings::ProgSettings(ProgSettings& ps)
{
    target = ps.target;
    device = std::move(ps.device);

    if (!ps.macrolist.empty())
    {
        macrolist = ps.macrolist;
    }
}

bool ProgSettings::Save() const
{
    bool success = true;

    std::unordered_map<std::string, unsigned short> targetData = { {"targetVID", target.targetVID},
                       {"targetPID", target.targetPID},
                       {"usagePage", target.usagePage},
                       {"usageCode", target.usageCode} };
    for (auto& [key, value] : targetData)
    {
        success &= RegistryHelper::WriteDwordValue(key, value, "TargetData");
    }

    if (device->getDevicePath().length() > 0)
    {
        success &= RegistryHelper::WriteStringValue("DevicePath", device->getDevicePath());
    }

    for (auto& [key, ma] : macrolist)
    {
        std::stringstream path;
        path << "Macros\\" << std::format("{:04x}", key);
        success &= RegistryHelper::WriteDwordValue("actionCode", static_cast<unsigned long>(ma.getActionCode()), path.str());
        success &= RegistryHelper::WriteStringValue("description", ma.getDescription(), path.str());
        switch (ma.getActionCode())
        {
            case MacroActionCode::String:
                success &= RegistryHelper::WriteStringValue("payload", ma.getString(), path.str());
                break;
            case MacroActionCode::VirtualKey: [[fallthrough]];
            case MacroActionCode::Char:
                success &= RegistryHelper::WriteDwordValue("payload", ma.getIntPayload(), path.str());
                break;
            default:
                success &= RegistryHelper::WriteDwordValue("payload", 0, path.str());
                break;
        }
    }
    return success;
}

HidDevicePtr ProgSettings::getDevice()
{
    return std::move(device);
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
                strm << std::format("    Payload: {:s}\n\n", Utils::virtualKeyStrings[action.getVK()]);
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

bool ProgSettings::Load()
{
    std::vector<std::string> values = RegistryHelper::EnumerateSubValues();
    std::string devicepath{};
    bool success{ false };   // stores overall status

    // DevicePath is preferable as we can create a device directly from it
    if (RegistryHelper::ReadStringValue("DevicePath", devicepath))
    {
        device = std::make_unique<HidDevice>(devicepath);
        target = device->getTargetInfo();
        success = true;
    }
    else if (RegistryHelper::CheckIfKeyExists("TargetData"))
    {
        unsigned long value{ 0 };
        bool noerror{ true };   // Stores the individual success status
        success = true;

        noerror = RegistryHelper::ReadDwordValue("targetVID", value, "TargetData");
        success &= noerror;
        target.targetVID = noerror ? static_cast<unsigned short>(value) : 0;

        noerror = RegistryHelper::ReadDwordValue("targetPID", value, "TargetData");
        success &= noerror;
        target.targetPID = noerror ? static_cast<unsigned short>(value) : 0;

        noerror = RegistryHelper::ReadDwordValue("usagePage", value, "TargetData");
        success &= noerror;
        target.usagePage = noerror ? static_cast<unsigned short>(value) : 0;

        noerror = RegistryHelper::ReadDwordValue("usageCode", value, "TargetData");
        success &= noerror;
        target.usageCode = noerror ? static_cast<unsigned short>(value) : 0;

        if (success)
        {
            success = false;

            HidDevices devices{};
            devices.FindAllHidDevices(true);
            std::vector<HidDevicePtr>& list = devices.getDevices();
            for (auto& dev : list)
            {
                if (dev->IsKnownTarget(target))
                {
                    device = std::move(dev);
                    success = true;
                    break;
                }
            }
        }
    }

    // if success is false, then DevicePath nor TargetData yielded a device to work with
    // Ask user if they want to run the setup function to locate a device.
    if (!success)
    {
        std::cerr << "No valid target device found in config." << std::endl;
        if (Utils::AskResponse("Do you want to run the setup function?", /*yesDefault = */ true))
        {
            auto newSetup = Setup::invokeSetup();
            target = newSetup->target;
            device = std::move(newSetup->device);
            if (!newSetup->macrolist.empty())
            {
                macrolist = newSetup->macrolist;
            }

            return Save();
        }
        else
        {
            std::cerr << "No valid configuration. Exiting." << std::endl;
            exit(false);
        }
    }

    auto macros = RegistryHelper::EnumerateSubKeys("Macros");

    for (const auto& macro : macros)
    {
        unsigned short key = static_cast<unsigned short>(std::stoi(macro, nullptr, 16));
        std::string keyname = "Macros\\" + macro;
        std::string description{};
        success &= RegistryHelper::ReadStringValue("description", description, keyname);
        unsigned long actioncode{ 0 };
        success &= RegistryHelper::ReadDwordValue("actionCode", actioncode, keyname);

        if (actioncode == 3)            // String
        {
            std::string payload{};
            success &= RegistryHelper::ReadStringValue("payload", payload, keyname);
            macrolist[key] = MacroAction(payload, description);
        }
        else
        {
            unsigned long payload{ 0 };
            success &= RegistryHelper::ReadDwordValue("payload", payload, keyname);
            if (actioncode == 1)        // Virtual Key
            {
                macrolist[key] = MacroAction(static_cast<short>(payload), description);
            }
            else if (actioncode == 2)   // Char
            {
                macrolist[key] = MacroAction(static_cast<char>(payload), description);
            }
        }
    }

    return success;
}
