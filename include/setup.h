#pragma once
#include <memory>
#include "ProgSettings.h"
#include "HidDevice.h"

class Setup
{
    static void OutputDeviceList(const HidDevices& devices);
public:
    static std::unique_ptr<ProgSettings> invokeSetup();
};