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
    this->ActionCode = MacroActionCode::Invalid;
}

MacroAction::MacroAction(char newPayload)
{
    this->ActionCode = MacroActionCode::Char;
    SetPayload(newPayload, NULL, "");
}

MacroAction::MacroAction(std::string newPayload)
{
    this->ActionCode = MacroActionCode::String;
    SetPayload(NULL, NULL, newPayload);
}

MacroAction::MacroAction(short newPayload)
{
    this->ActionCode = MacroActionCode::VirtualKey;
    SetPayload(NULL, newPayload, "");
}
MacroAction::~MacroAction()
{
}

MacroActionCode MacroAction::GetActionCode() const
{
    return this->ActionCode;
}

char MacroAction::GetChar() const
{
    return this->_charPayload;
}

std::string MacroAction::GetString() const
{
    return this->_strPayload;
}

short MacroAction::GetVK() const
{
    return this->_vkPayload;
}
