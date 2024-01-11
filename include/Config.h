#pragma once
#include <string>

class Config
{
public:
    Config();
    Config(std::string filename);
    ~Config();

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

