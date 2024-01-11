#pragma once
#include "Alien-Macros.h"
#include "MacroAction.h"
#include <unordered_map>

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
