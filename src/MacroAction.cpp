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

using namespace std::string_literals;   // Required for string literal suffix

/// <summary>
/// Sets the payload values
/// </summary>
/// <param name="c">char value payload</param>
/// <param name="vk">Virtual Key code</param>
/// <param name="s">std::string payload</param>
void MacroAction::SetPayload(const char c, const short vk, const std::string& s)
{
    this->_charPayload = c;
    this->_vkPayload = vk;
    this->_strPayload = s;
}

/// <summary>
/// Creates an indeterminate action with no payload and an Invalid action code
/// </summary>
MacroAction::MacroAction()
{
    SetPayload(NULL, NULL, ""s);
    this->description = "";
    this->ActionCode = MacroActionCode::Invalid;
}

/// <summary>
/// Create a char action and loads the payload to the proper variable.
/// </summary>
/// <param name="newPayload">Value to be loaded into payload</param>
/// <param name="desc">Description for this action</param>
MacroAction::MacroAction(const char newPayload, const std::string& desc)
{
    this->description = desc;
    this->ActionCode = MacroActionCode::Char;
    SetPayload(newPayload, NULL, ""s);
}

/// <summary>
/// Create a std::string action and loads the payload to the proper variable.
/// </summary>
/// <param name="newPayload">std::string value to be loaded</param>
/// <param name="desc">Description for this action</param>
MacroAction::MacroAction(const std::string& newPayload, const std::string& desc)
{
    this->description = desc;
    this->ActionCode = MacroActionCode::String;
    SetPayload(NULL, NULL, newPayload);
}

/// <summary>
/// Creates a Virtual key action 
/// </summary>
/// <param name="newPayload">Value to be loaded to the proper variable</param>
/// <param name="desc">Description for this action</param>
MacroAction::MacroAction(const short newPayload, const std::string& desc)
{
    this->description = desc;
    this->ActionCode = MacroActionCode::VirtualKey;
    SetPayload(NULL, newPayload, ""s);
}

/// <summary>
/// Retrieves action enum code
/// </summary>
/// <returns>MacroActionCode representing action to be performed</returns>
MacroActionCode MacroAction::getActionCode() const
{
    return this->ActionCode;
}

/// <summary>
/// If this is a char action, returns the char variable
/// </summary>
/// <returns>Char payload if action is Char. Blank otherwise.</returns>
char MacroAction::getChar() const
{
    if (this->ActionCode == MacroActionCode::Char)
    {
        return this->_charPayload;
    }
    return {};
}

/// <summary>
/// If this is a string action, returns the string variable
/// </summary>
/// <returns>String payload if action is a string. Blank otherwise.</returns>
std::string MacroAction::getString() const
{
    if (this->ActionCode == MacroActionCode::String)
    {
        return this->_strPayload;
    }
    return {};
}

/// <summary>
/// If this is a virtual key action, returns the virtual key variable
/// </summary>
/// <returns>Virtual Key payload if action is VirtualKey. Blank (0) otherwise.</returns>
short MacroAction::getVK() const
{
    if (this->ActionCode == MacroActionCode::VirtualKey)
    {
        return this->_vkPayload;
    }
    return {};
}

/// <summary>
/// Retrieves description for this action
/// </summary>
/// <returns>std::string description for this action</returns>
std::string MacroAction::getDescription() const
{
    return this->description;
}
