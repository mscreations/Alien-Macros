#pragma once
#include <memory>
#include "ProgSettings.h"
#include "HidDevice.h"

class Setup
{
    static MacroAction SelectMacroAction(const std::string&);
    static void OutputDeviceList(const HidDevices& devices);
    static HidDevicePtr ChooseTarget();
    static bool VerifyTarget(HidDevicePtr& target);
public:
    static std::unique_ptr<ProgSettings> invokeSetup();
};