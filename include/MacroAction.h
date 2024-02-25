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
#include <iostream>
#include <variant>

 /// <summary>
 /// Action to be performed by MacroAction
 /// </summary>
enum class MacroActionCode : int
{
    Invalid,
    VirtualKey,
    Char,
    String
};

/// <summary>
/// Object that contains the actions to be performed when a macro key is pressed.
/// </summary>
/// <remarks>Needs to be expanded in a future version</remarks>
class MacroAction
{
    MacroActionCode ActionCode;
    std::variant<char, short, std::string> payload;
    std::string     description;

public:
    explicit MacroAction(const std::string& desc = "");
    explicit MacroAction(const char newPayload, const std::string& desc);
    explicit MacroAction(const std::string& newPayload, const std::string& desc);
    explicit MacroAction(const short newPayload, const std::string& desc);
    MacroActionCode getActionCode() const;
    int getIntPayload() const;
    char getChar() const;
    std::string getString() const;
    short getVK() const;
    std::string getDescription() const;
    friend std::ostream& operator<<(std::ostream& strm, const MacroAction& ma);
};

