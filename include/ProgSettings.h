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

#pragma once
#include "libconfig.h++"
#include "MacroAction.h"
#include "Utils.h"
#include <cxxopts.hpp>
#include <string>
#include <unordered_map>

constexpr auto DEFAULT_CFG_FILENAME = "awmacros.cfg";

// configuration file section labels
constexpr auto TARGET_DEVICE = "targetdevice";
constexpr auto VID = "vid";
constexpr auto PID = "pid";
constexpr auto USAGEPAGE = "usagepage";
constexpr auto USAGECODE = "usagecode";
constexpr auto MACRO_COUNT = "macrocount";
constexpr auto MACROS = "macros";
constexpr auto SCANCODE = "scancode";
constexpr auto PAYLOAD_TYPE = "payloadtype";
constexpr auto MACRO_ACTION = "macroaction";
constexpr auto MACRO_PAYLOAD = "macropayload";
constexpr auto MACRO_KEY_DESCRIPTION = "macrodescription";

class ProgSettings
{
public:
    ProgSettings(int argc, char* argv[]);

    friend std::ostream& operator<<(std::ostream& strm, const ProgSettings& ps);

    void CreateBlank();

    int getVID() const;
    int getPID() const;
    int getUsagePage() const;
    int getUsageCode() const;
    std::string getDescription(const short scancode) const;
    std::unordered_map<short, MacroAction> getMacros() const;

private:
    int targetVID;
    int targetPID;
    int usagePage;
    int usageCode;
    std::unordered_map<short, MacroAction> macrolist;
    std::string configFilename;
    libconfig::Config configuration;

    bool Load(const std::string filename);
    bool Save();
};

