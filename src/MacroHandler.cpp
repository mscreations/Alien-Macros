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

#include "MacroHandler.h"
#include <vector>

MacroHandler::MacroHandler()
{
    // Default handler sets the 4 macro keys from the Alienware m17 R4 to be remapped to F13-F16
    macroKeys[0x4c] = MacroAction((short)VK_F13, "Macro A");
    macroKeys[0x4d] = MacroAction((short)VK_F14, "Macro B");
    macroKeys[0x4e] = MacroAction((short)VK_F15, "Macro C");
    macroKeys[0x4f] = MacroAction((short)VK_F16, "Macro D");
}

MacroHandler::MacroHandler(const std::unordered_map<short, MacroAction> macros)
{
    macroKeys = macros;
}

MacroHandler::~MacroHandler() { macroKeys.clear(); }

void MacroHandler::Process(const USAGE macroKey)
{
    MacroAction action = macroKeys[macroKey];
    switch (action.getActionCode())
    {
        case MacroActionCode::VirtualKey:
            Send(action.getVK(), false);
            break;
        case MacroActionCode::Char:
            Send(action.getChar());
            break;
        case MacroActionCode::String:
            Send(action.getString());
            break;
        default:
            break;
    }
}

bool MacroHandler::Send(const WORD wVk, const bool shift) const
{
    std::vector<INPUT> inputs = GetKeystrokes(wVk, shift);
    int sentCount = SendInput(static_cast<UINT>(inputs.size()), &inputs[0], sizeof(INPUT));
    return (sentCount == inputs.size());
}

bool MacroHandler::Send(const char outChar) const
{
    SHORT vk = VkKeyScanExA(outChar, GetKeyboardLayout(0));
    return Send(static_cast<WORD>((vk & 0xFF)), static_cast<bool>((vk & 0x0100) >> 8));
}

bool MacroHandler::Send(const std::string outputString) const
{
    std::vector<INPUT> inputs;

    for (const char& c : outputString)
    {
        std::vector<INPUT> charInputs = GetKeystrokes(c);
        inputs.insert(inputs.end(), charInputs.begin(), charInputs.end());
    }
    int SentCount = SendInput(static_cast<UINT>(inputs.size()), &inputs[0], sizeof(INPUT));
    return (SentCount == inputs.size());
}

std::vector<INPUT> MacroHandler::GetKeystrokes(const WORD wVk, const bool shift) const
{
    std::vector<INPUT> inputs;
    INPUT shiftInput{ .type = INPUT_KEYBOARD, .ki = {VK_LSHIFT, 0, 0, 0, 0} };
    INPUT charInput{ .type = INPUT_KEYBOARD, .ki = {wVk, 0, 0, 0, 0 } };

    if (shift)
    {
        inputs.push_back(shiftInput);
    }

    inputs.push_back(charInput);

    charInput.ki.dwFlags = KEYEVENTF_KEYUP;
    inputs.push_back(charInput);

    if (shift)
    {
        shiftInput.ki.dwFlags = KEYEVENTF_KEYUP;
        inputs.push_back(shiftInput);
    }

    return inputs;
}

std::vector<INPUT> MacroHandler::GetKeystrokes(const char outChar) const
{
    SHORT vk = VkKeyScanExA(outChar, GetKeyboardLayout(0));
    return GetKeystrokes(static_cast<WORD>((vk & 0xFF)), static_cast<bool>((vk & 0x0100) >> 8));
}
