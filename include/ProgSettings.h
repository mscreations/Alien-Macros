#pragma once
#include <string>
#include "libconfig.h++"

#define DEFAULT_CFG_FILENAME    "awmacros.cfg"

class ProgSettings
{
public:
    ProgSettings();
    ProgSettings(std::string filename);
    ~ProgSettings();

    void CreateBlank();
    void SetVID(short newVID);
    void SetPID(short newPID);
    short GetVID();
    short GetPID();

private:
    short targetVID;
    short targetPID;
    std::string configFilename;

    bool Load(std::string filename);
    bool Save();
};

