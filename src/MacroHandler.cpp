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
        case MA_VirtualKey:
            Send(action.GetVK(), false);
            break;
        case MA_Char:
            Send(action.GetChar());
            break;
        case MA_String:
            Send(action.GetString());
            break;
    }
}

bool MacroHandler::Send(WORD wVk, bool shift)
{
    std::vector<INPUT> inputs(shift ? 4 : 2);
    std::memset(&inputs[0], 0, inputs.size() * sizeof(INPUT));
    size_t i = 0;
    if (shift)
    {
        inputs[i].type = INPUT_KEYBOARD;
        inputs[i].ki.wVk = VK_LSHIFT;
        i++;
    }

    inputs[i].type = INPUT_KEYBOARD;
    inputs[i++].ki.wVk = wVk;

    inputs[i] = inputs[i - 1];
    inputs[i++].ki.dwFlags = KEYEVENTF_KEYUP;

    if (shift)
    {
        inputs[i] = inputs[0];
        inputs[i].ki.dwFlags = KEYEVENTF_KEYUP;
    }

    int sentCount = SendInput((UINT)inputs.size(), &inputs[0], sizeof(INPUT));
    return (sentCount == inputs.size());
}

bool MacroHandler::Send(char outChar)
{
    // This assumes US keyboard layout. Not sure if other people might need the Keyboard Layout to be changed in order to properly send characters.
    SHORT vk = VkKeyScanExA(outChar, LoadKeyboardLayoutA("00000409", 0));
    return Send((WORD)(vk & 0xFF), (bool)((vk & 0xFF00) >> 8));
}

bool MacroHandler::Send(std::string outputString)
{
    for (char& c : outputString)
    {
        if (!Send(c))
        {
            return false;
        }
        Sleep(20);          // Without the delay, the whole string won't show up unless the key is pressed again.
        // 20 ms seems good for my system, but this may need adjusted for other people.
    }
    return true;
}
