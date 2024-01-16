#include "argparse.h"
#include "ProgSettings.h"
#include <format>
#include <iostream>
#include <regex>

using namespace libconfig;
using namespace std;

ProgSettings::ProgSettings()
{
    // LOAD CONFIG FROM NORMAL LOCATION (WHEREVER THAT MAY END UP BEING)
    targetPID = 0x0d62;
    targetVID = 0x1a1c;
    usageCode = 0x1;
    usagePage = 0xc;

}

ProgSettings::ProgSettings(int argc, char* argv[])
{
    argparse::Parser parser;

    auto vid = parser.AddArg<std::string>("vid", 'v', "Target VID");
    auto pid = parser.AddArg<std::string>("pid", 'p', "Target PID");
    auto& ConfigFile = parser.AddArg<std::string>("config", 'c', "Configuration File").Default(DEFAULT_CFG_FILENAME);

    parser.ParseArgs(argc, argv);

    short svid{};
    short spid{};

    if (vid && pid)
    {
        std::regex re("(?:0x)[0-9a-fA-F]{4}");
        if (!std::regex_match(*pid, re) || !std::regex_match(*vid, re))
        {
            throw std::invalid_argument("VID/PID is invalid. Please make sure it is in the format 0xXXXX where XXXX is the hexadecimal VID/PID.");
        }
        svid = static_cast<short>(std::stoi(*vid, nullptr, 16));
        spid = static_cast<short>(std::stoi(*pid, nullptr, 16));
    }

    if (!Load(*ConfigFile))
    {
        throw std::invalid_argument("Configuration file could not be loaded");
    }

    // if command line arguments for VID and PID are different than configuration, use them instead.
    if (svid != 0 && targetVID != svid) { targetVID = svid; }
    if (spid != 0 && targetPID != spid) { targetPID = spid; }
}

ProgSettings::ProgSettings(string filename)
{
    Load(filename);
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

int ProgSettings::getVID() const
{
    return targetVID;
}

int ProgSettings::getPID() const
{
    return targetPID;
}

int ProgSettings::getUsagePage() const
{
    return usagePage;
}

int ProgSettings::getUsageCode() const
{
    return usageCode;
}

bool ProgSettings::Save()
{
    return true;
}

bool ProgSettings::Load(string filename)
{
    configFilename = filename;

    // Load and parse the file with libconfig
    try
    {
        configuration.readFile(filename);
    }
    catch (const FileIOException& fioex)
    {
        cerr << "IO error reading file" << endl;
        cerr << fioex.what() << endl;
        return false;
    }
    catch (const ParseException& pex)
    {
        cerr << format("Parse error at {}:{} - {}", pex.getFile(), pex.getLine(), pex.getError()) << endl;
        return false;
    }

    try
    {
        // Extract targetdevice details from configuration
        Setting& target = configuration.lookup(TARGET_DEVICE);
        targetVID = target[VID];
        targetPID = target[PID];
        usagePage = target[USAGEPAGE];
        usageCode = target[USAGECODE];

        // Extract macro information from configuration
        Setting& macros = configuration.lookup(MACROS);

        for (Setting& macro : macros)
        {
            int scancode{ 0 }, payloadtype{ 0 }, actioncode{ 0 }, intPayload{ 0 };
            string stringPayload{};

            macro.lookupValue(SCANCODE, scancode);
            macro.lookupValue(PAYLOAD_TYPE, payloadtype);
            macro.lookupValue(MACRO_ACTION, actioncode);

            if (payloadtype == Setting::TypeString)
            {
                macro.lookupValue(MACRO_PAYLOAD, stringPayload);
                macrolist[scancode] = MacroAction(stringPayload);
            }
            else if (payloadtype == Setting::TypeInt)
            {
                macro.lookupValue(MACRO_PAYLOAD, intPayload);

                switch (static_cast<MacroActionCode>(actioncode))
                {
                    case MacroActionCode::VirtualKey:
                        macrolist[scancode] = MacroAction(static_cast<short>(intPayload));
                        break;
                    case MacroActionCode::Char:
                        macrolist[scancode] = MacroAction(static_cast<char>(intPayload));
                        break;
                }
            }
        }
    }
    catch (const SettingNotFoundException& snfe)
    {
        cerr << format("{} - Could not load configuration information:\nMissing Path: {}", snfe.what(), snfe.getPath()) << endl;
        return false;
    }
    catch (const SettingTypeException& ste)
    {
        cerr << format("{} - Invalid Type\nPath: {}", ste.what(), ste.getPath()) << endl;
        return false;
    }

    return true;
}