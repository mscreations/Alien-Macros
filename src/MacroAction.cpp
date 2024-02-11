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
#include "Utils.h"

 /// <summary>
 /// Creates an indeterminate action with no payload and an Invalid action code
 /// </summary>
MacroAction::MacroAction(const std::string& desc)
{
    payload = "";
    description = desc;
    ActionCode = MacroActionCode::Invalid;
}

/// <summary>
/// Create a char action and loads the payload to the proper variable.
/// </summary>
/// <param name="newPayload">Value to be loaded into payload</param>
/// <param name="desc">Description for this action</param>
MacroAction::MacroAction(const char newPayload, const std::string& desc)
{
    description = desc;
    ActionCode = MacroActionCode::Char;
    payload = newPayload;
}

/// <summary>
/// Create a std::string action and loads the payload to the proper variable.
/// </summary>
/// <param name="newPayload">std::string value to be loaded</param>
/// <param name="desc">Description for this action</param>
MacroAction::MacroAction(const std::string& newPayload, const std::string& desc)
{
    description = desc;
    ActionCode = MacroActionCode::String;
    payload = newPayload;
}

/// <summary>
/// Creates a Virtual key action 
/// </summary>
/// <param name="newPayload">Value to be loaded to the proper variable</param>
/// <param name="desc">Description for this action</param>
MacroAction::MacroAction(const short newPayload, const std::string& desc)
{
    description = desc;
    ActionCode = MacroActionCode::VirtualKey;
    payload = newPayload;
}

/// <summary>
/// Retrieves action enum code
/// </summary>
/// <returns>MacroActionCode representing action to be performed</returns>
MacroActionCode MacroAction::getActionCode() const
{
    return ActionCode;
}

/// <summary>
/// If this is a char action, returns the char variable
/// </summary>
/// <returns>Char payload if action is Char. Blank otherwise.</returns>
char MacroAction::getChar() const
{
    if (ActionCode == MacroActionCode::Char &&
        std::holds_alternative<char>(payload))
    {
        return std::get<char>(payload);
    }
    return {};
}

/// <summary>
/// If this is a string action, returns the string variable
/// </summary>
/// <returns>String payload if action is a string. Blank otherwise.</returns>
std::string MacroAction::getString() const
{
    if (ActionCode == MacroActionCode::String &&
        std::holds_alternative<std::string>(payload))
    {
        return std::get<std::string>(payload);
    }
    return {};
}

/// <summary>
/// If this is a virtual key action, returns the virtual key variable
/// </summary>
/// <returns>Virtual Key payload if action is VirtualKey. Blank (0) otherwise.</returns>
short MacroAction::getVK() const
{
    if (ActionCode == MacroActionCode::VirtualKey &&
        std::holds_alternative<short>(payload))
    {
        return std::get<short>(payload);
    }
    return {};
}

/// <summary>
/// Retrieves description for this action
/// </summary>
/// <returns>std::string description for this action</returns>
std::string MacroAction::getDescription() const
{
    return description;
}

std::ostream& operator<<(std::ostream& strm, const MacroAction& ma)
{
    switch (ma.ActionCode)
    {
        case MacroActionCode::String:
            strm << "String action: " << ma.getString(); break;
        case MacroActionCode::Char: strm << "Char action: " << ma.getChar(); break;
        case MacroActionCode::VirtualKey: strm << "Virtual Key: " << Utils::virtualKeyStrings[ma.getVK()]; break;
        case MacroActionCode::Invalid: strm << "Invalid action"; break;
    }
    return strm;
}
