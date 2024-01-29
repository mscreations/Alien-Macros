#include <iostream>

#include <iomanip>
#include "setup.h"
#include "colors.h"

bool Setup::AskResponse(const std::string& message, const unsigned int firstOption, const unsigned int lastOption, unsigned int& response)
{
    std::cout << std::format("{} [{} - {}]: ", message, firstOption, lastOption);
    std::cin >> std::skipws >> response;

    if (response < firstOption || response > lastOption)
    {
        response = -1;          // Invalid value
        std::cout << Colors::CursorPreviousLine;    // Move cursor back to start of line 
        return false;
    }
    return true;
}

void Setup::OutputDeviceList(const HidDevices& devices)
{
    for (int i = 1; const auto & dev : devices.getDevices())
    {
        bool known = false;
        if (ProgSettings::knownDevices.find(dev->getTargetInfo()) != ProgSettings::knownDevices.end())
        {
            known = true;
        }
        std::cout << (known ? Colors::Green : "") << std::setw(3) << i++ << ": " << *dev.get() << (known ? Colors::Reset : "");
    }
}

std::unique_ptr<ProgSettings> Setup::invokeSetup()
{
    // TODO: insert return statement here
    auto ps = std::make_unique<ProgSettings>();
    HidDevices devices{};
    devices.FindAllHidDevices(true);

    std::cout << Colors::ClearScreen << Colors::Green << "\n\nConnected HID Devices:\n" << Colors::Reset;
    OutputDeviceList(devices);

    std::cout << std::endl;

    bool valid = false;
    unsigned int response{ 0 };

    while (!AskResponse("Select device to target (Green are known devices)", 1, static_cast<unsigned int>(devices.size()), response)) {};

    std::cout << "Selected Target device:\n" << devices[response - 1].getTargetInfo() << std::endl;

    return ps;
}
