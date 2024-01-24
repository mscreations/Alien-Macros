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
#include <string>

enum class MacroActionCode : int
{
    Invalid,
    VirtualKey,
    Char,
    String
};

class MacroAction
{
    MacroActionCode ActionCode;
    char            _charPayload;
    std::string     _strPayload;
    short           _vkPayload;
    std::string     description;
    void SetPayload(char, short, std::string);

public:
    MacroAction();
    MacroAction(const char newPayload, const std::string desc);
    MacroAction(const std::string newPayload, const std::string desc);
    MacroAction(const short newPayload, const std::string desc);
    MacroActionCode getActionCode() const;
    char getChar() const;
    std::string getString() const;
    short getVK() const;
    std::string getDescription() const;
};

