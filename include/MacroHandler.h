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
#include <unordered_map>
#include <wtypes.h>
#include "hid.h"
#include "MacroAction.h"

class MacroHandler
{
public:
    MacroHandler();
    ~MacroHandler();
    void Process(USAGE macroKey);
private:
    std::unordered_map<short, MacroAction> macroKeys;
    bool Send(WORD wVk, bool shift);
    bool Send(char outChar);
    bool Send(std::string outputString);
    std::vector<INPUT> GetKeystrokes(WORD wVk, bool shift);
    std::vector<INPUT> GetKeystrokes(char outChar);
};
