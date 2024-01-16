#include "MacroHandler.h"
#include <vector>

MacroHandler::MacroHandler()
{
    // TODO
    macroKeys[0x4c] = MacroAction((short)VK_F13);
    macroKeys[0x4d] = MacroAction((short)VK_F14);
    macroKeys[0x4e] = MacroAction((short)VK_F15);
    macroKeys[0x4f] = MacroAction((short)VK_F16);
}

MacroHandler::~MacroHandler()
{
    macroKeys.clear();
}

void MacroHandler::Process(USAGE macroKey)
{
    MacroAction action = macroKeys[macroKey];
    switch (action.GetActionCode())
    {
        case MacroActionCode::VirtualKey:
            Send(action.GetVK(), false);
            break;
        case MacroActionCode::Char:
            Send(action.GetChar());
            break;
        case MacroActionCode::String:
            Send(action.GetString());
            break;
    }
}

bool MacroHandler::Send(WORD wVk, bool shift)
{
    std::vector<INPUT> inputs = GetKeystrokes(wVk, shift);
    int sentCount = SendInput(static_cast<UINT>(inputs.size()), &inputs[0], sizeof(INPUT));
    return (sentCount == inputs.size());
}

bool MacroHandler::Send(char outChar)
{
    SHORT vk = VkKeyScanExA(outChar, GetKeyboardLayout(0));
    return Send(static_cast<WORD>((vk & 0xFF)), static_cast<bool>((vk & 0x0100) >> 8));
}

bool MacroHandler::Send(std::string outputString)
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

std::vector<INPUT> MacroHandler::GetKeystrokes(WORD wVk, bool shift)
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

std::vector<INPUT> MacroHandler::GetKeystrokes(char outChar)
{
    SHORT vk = VkKeyScanExA(outChar, GetKeyboardLayout(0));
    return GetKeystrokes(static_cast<WORD>((vk & 0xFF)), static_cast<bool>((vk & 0x0100) >> 8));
}
