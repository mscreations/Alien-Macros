#include "ProgSettings.h"

ProgSettings::ProgSettings()
{
    // LOAD CONFIG FROM NORMAL LOCATION (WHEREVER THAT MAY END UP BEING)
    targetPID = 0x0d62;
    targetVID = 0x1a1c;

}

ProgSettings::ProgSettings(std::string filename)
{
    // LOAD SPECIFIED CONFIG FROM FILENAME
    targetPID = 0x0d62;
    targetVID = 0x1a1c;

}

ProgSettings::~ProgSettings()
{
    // SAVE CONFIG TO FILE
}

void ProgSettings::CreateBlank()
{
    // TODO: Add your implementation code here.
    targetPID = 0x0d62;
    targetVID = 0x1a1c;
}

void ProgSettings::SetVID(short newVID)
{
    targetVID = newVID;
}

void ProgSettings::SetPID(short newPID)
{
    targetPID = newPID;
}

short ProgSettings::GetVID() const
{
    return targetVID;
}

short ProgSettings::GetPID() const
{
    return targetPID;
}

bool ProgSettings::Save()
{
    return true;
}

bool ProgSettings::Load(std::string filename)
{
    return true;
}