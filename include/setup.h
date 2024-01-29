#pragma once
#include <memory>
#include "ProgSettings.h"
#include "HidDevice.h"

class Setup
{
    static bool AskResponse(const std::string& message, const unsigned int first, const unsigned int last, unsigned int& response);

    static void OutputDeviceList(const HidDevices& devices);
public:
    static std::unique_ptr<ProgSettings> invokeSetup();
};