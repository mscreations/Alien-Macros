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

#include "MacroAction.h"

void MacroAction::SetPayload(const char c, const short vk, const std::string s)
{
    this->_charPayload = c;
    this->_vkPayload = vk;
    this->_strPayload = s;
}

MacroAction::MacroAction()
{
    SetPayload(NULL, NULL, "");
    this->description = "";
    this->ActionCode = MacroActionCode::Invalid;
}

MacroAction::MacroAction(const char newPayload, const std::string desc)
{
    this->description = desc;
    this->ActionCode = MacroActionCode::Char;
    SetPayload(newPayload, NULL, "");
}

MacroAction::MacroAction(const std::string newPayload, const std::string desc)
{
    this->description = desc;
    this->ActionCode = MacroActionCode::String;
    SetPayload(NULL, NULL, newPayload);
}

MacroAction::MacroAction(const short newPayload, const std::string desc)
{
    this->description = desc;
    this->ActionCode = MacroActionCode::VirtualKey;
    SetPayload(NULL, newPayload, "");
}

MacroActionCode MacroAction::getActionCode() const
{
    return this->ActionCode;
}

char MacroAction::getChar() const
{
    if (this->ActionCode == MacroActionCode::Char)
    {
        return this->_charPayload;
    }
    return {};
}

std::string MacroAction::getString() const
{
    if (this->ActionCode == MacroActionCode::String)
    {
        return this->_strPayload;
    }
    return {};
}

short MacroAction::getVK() const
{
    if (this->ActionCode == MacroActionCode::VirtualKey)
    {
        return this->_vkPayload;
    }
    return {};
}

std::string MacroAction::getDescription() const
{
    return this->description;
}
