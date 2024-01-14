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
};
