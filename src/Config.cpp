#include "Config.h"

Config::Config()
{
    // LOAD CONFIG FROM NORMAL LOCATION (WHEREVER THAT MAY END UP BEING)
    targetPID = 0x0d62;
    targetVID = 0x1a1c;

}

Config::Config(std::string filename) 
{
    // LOAD SPECIFIED CONFIG FROM FILENAME
    targetPID = 0x0d62;
    targetVID = 0x1a1c;
    
}

Config::~Config()
{
    // SAVE CONFIG TO FILE
}

void Config::CreateBlank()
{
    // TODO: Add your implementation code here.
    targetPID = 0x0d62;
    targetVID = 0x1a1c;
}

void Config::SetVID(short newVID)
{
    targetVID = newVID;
}

void Config::SetPID(short newPID)
{   
    targetPID = newPID;
}

short Config::GetVID()
{
    return targetVID;
}

short Config::GetPID()
{
    return targetPID;
}

bool Config::Save()
{
    return true;
}

bool Config::Load(std::string filename)
{
    return true;
}