#include "argparse.h"
#include "ProgSettings.h"
#include <format>
#include <iomanip>
#include <regex>

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

std::ostream& operator<<(std::ostream& strm, const ProgSettings& ps)
{
    strm << "CONFIGURATION:" << std::endl;
    strm << std::endl << "TargetDevice:" << std::endl;
    strm << std::format("VID:        {:#06x}\nPID:        {:#06x}\nUsagePage:  {:#04x}\nUsage:      {:#04x}",
                   ps.targetVID, ps.targetPID, ps.usagePage, ps.usageCode) << std::endl << std::endl;

    strm << "Macro Keys: " << ps.macrolist.size() << std::endl << std::endl;

    for (const auto& macro : ps.macrolist)
    {
        strm << "Macro Code: 0x" << std::hex << macro.first << std::endl;
        MacroAction action = static_cast<MacroAction>(macro.second);
        switch (action.GetActionCode())
        {
            case MacroActionCode::VirtualKey:
                strm << "Virtual Key Action" << std::endl;
                strm << "Virtual Key Payload: " << std::left << std::setw(10) << GetKeyName(action.GetVK()) << std::endl;
                break;
            case MacroActionCode::Char:
                strm << "Char Action" << std::endl;
                strm << "Char Payload: " << action.GetChar() << std::endl;
                break;
            case MacroActionCode::String:
                strm << "String Action" << std::endl;
                strm << "String Payload: " << action.GetString() << std::endl;
                break;
            default:
                strm << "Unknown/Undefined action code" << std::endl;
                break;
        }
        strm << std::endl;
    }
    return strm;
}

void ProgSettings::CreateBlank()
{
    // TODO: Add your implementation code here.
    targetPID = 0x0d62;
    targetVID = 0x1a1c;
}

int ProgSettings::getVID() const { return targetVID; }
int ProgSettings::getPID() const { return targetPID; }
int ProgSettings::getUsagePage() const { return usagePage; }
int ProgSettings::getUsageCode() const { return usageCode; }

bool ProgSettings::Save()
{
    return true;
}

bool ProgSettings::Load(std::string filename)
{
    using namespace libconfig;

    configFilename = filename;

    // Load and parse the file with libconfig
    try
    {
        configuration.readFile(filename);
    }
    catch (const FileIOException& fioex)
    {
        std::cerr << "IO error reading file" << std::endl;
        std::cerr << fioex.what() << std::endl;
        return false;
    }
    catch (const ParseException& pex)
    {
        std::cerr << std::format("Parse error at {}:{} - {}", pex.getFile(), pex.getLine(), pex.getError()) << std::endl;
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
            std::string stringPayload{};

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
        std::cerr << std::format("{} - Could not load configuration information:\nMissing Path: {}", snfe.what(), snfe.getPath()) << std::endl;
        return false;
    }
    catch (const SettingTypeException& ste)
    {
        std::cerr << std::format("{} - Invalid Type\nPath: {}", ste.what(), ste.getPath()) << std::endl;
        return false;
    }

    return true;
}