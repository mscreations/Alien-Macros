#pragma once
#include <iostream>

enum MacroActionCode
{
    MA_Invalid,
    MA_VirtualKey,
    MA_Char,
    MA_String
};

class MacroAction
{
    MacroActionCode ActionCode;
    char            _charPayload;
    std::string     _strPayload;
    short           _vkPayload;
    void SetPayload(char, short, std::string);

public:
    MacroAction();
    MacroAction(char newPayload);
    MacroAction(std::string newPayload);
    MacroAction(short newPayload);
    ~MacroAction();
    MacroActionCode GetActionCode();
    char GetChar();
    std::string GetString();
    short GetVK();
};

