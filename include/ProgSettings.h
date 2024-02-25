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

#include "MacroAction.h"
#include "Utils.h"
#include <string>
#include <unordered_map>
#include "TargetDevice.h"
#include "HidDevice.h"

class ProgSettings
{
    TargetDevice target;
    HidDevicePtr device;
    std::unordered_map<short, MacroAction> macrolist;
    std::string configFilename;

    [[nodiscard]] bool Load();
    [[nodiscard]] bool Save() const;

public:
    ProgSettings(bool skipAll = false);
    ProgSettings(ProgSettings& ps);

    HidDevicePtr getDevice();

    unsigned short getVID() const;
    unsigned short getPID() const;
    unsigned short getUsagePage() const;
    unsigned short getUsageCode() const;
    std::string getDescription(const short scancode) const;
    std::unordered_map<short, MacroAction> getMacros() const;

    friend std::ostream& operator<<(std::ostream& strm, const ProgSettings& ps);
    friend class Setup;     // Setup needs access to private members
};

