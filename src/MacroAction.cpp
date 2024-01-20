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

void MacroAction::SetPayload(char c, short vk, std::string s)
{
    this->_charPayload = c;
    this->_vkPayload = vk;
    this->_strPayload = s;
}

MacroAction::MacroAction()
{
    SetPayload(NULL, NULL, "");
    this->ActionCode = MacroActionCode::Invalid;
}

MacroAction::MacroAction(char newPayload)
{
    this->ActionCode = MacroActionCode::Char;
    SetPayload(newPayload, NULL, "");
}

MacroAction::MacroAction(std::string newPayload)
{
    this->ActionCode = MacroActionCode::String;
    SetPayload(NULL, NULL, newPayload);
}

MacroAction::MacroAction(short newPayload)
{
    this->ActionCode = MacroActionCode::VirtualKey;
    SetPayload(NULL, newPayload, "");
}
MacroAction::~MacroAction()
{
}

MacroActionCode MacroAction::GetActionCode() const
{
    return this->ActionCode;
}

char MacroAction::GetChar() const
{
    return this->_charPayload;
}

std::string MacroAction::GetString() const
{
    return this->_strPayload;
}

short MacroAction::GetVK() const
{
    return this->_vkPayload;
}
