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
    this->ActionCode = MA_Invalid;
}

MacroAction::MacroAction(char newPayload)
{
    this->ActionCode = MA_Char;
    SetPayload(newPayload, NULL, "");
}

MacroAction::MacroAction(std::string newPayload)
{
    this->ActionCode = MA_String;
    SetPayload(NULL, NULL, newPayload);
}

MacroAction::MacroAction(short newPayload)
{
    this->ActionCode = MA_VirtualKey;
    SetPayload(NULL, newPayload, "");
}
MacroAction::~MacroAction()
{
}

MacroActionCode MacroAction::GetActionCode()
{
    return this->ActionCode;
}

char MacroAction::GetChar()
{
    return this->_charPayload;
}

std::string MacroAction::GetString()
{
    return this->_strPayload;
}

short MacroAction::GetVK()
{
    return this->_vkPayload;
}
