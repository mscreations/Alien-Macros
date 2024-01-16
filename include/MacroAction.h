#pragma once
#include <iostream>

enum class MacroActionCode : int
{
    Invalid,
    VirtualKey,
    Char,
    String
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
    MacroActionCode GetActionCode() const;
    char GetChar() const;
    std::string GetString() const;
    short GetVK() const;
};

