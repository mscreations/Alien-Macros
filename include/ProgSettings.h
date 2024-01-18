#pragma once
#include "libconfig.h++"
#include "MacroAction.h"
#include <string>
#include <unordered_map>

constexpr auto DEFAULT_CFG_FILENAME = "awmacros.cfg";

// configuration file section labels
constexpr auto TARGET_DEVICE = "targetdevice";
constexpr auto VID = "vid";
constexpr auto PID = "pid";
constexpr auto USAGEPAGE = "usagepage";
constexpr auto USAGECODE = "usagecode";
constexpr auto MACRO_COUNT = "macrocount";
constexpr auto MACROS = "macros";
constexpr auto SCANCODE = "scancode";
constexpr auto PAYLOAD_TYPE = "payloadtype";
constexpr auto MACRO_ACTION = "macroaction";
constexpr auto MACRO_PAYLOAD = "macropayload";

#define AW_KB_VID       "0x0d62"
#define AW_KB_PID       "0x1a1c"

class ProgSettings
{
public:
    ProgSettings(int argc, char* argv[]);
    ProgSettings(string filename);
    ~ProgSettings();

    void CreateBlank();

    int getVID() const;
    int getPID() const;
    int getUsagePage() const;
    int getUsageCode() const;

private:
    int targetVID;
    int targetPID;
    int usagePage;
    int usageCode;
    std::unordered_map<int, MacroAction> macrolist;
    std::string configFilename;
    libconfig::Config configuration;

    bool Load(std::string filename);
    bool Save();
};

