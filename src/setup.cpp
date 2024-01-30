#include <iostream>
#include <iomanip>
#include "setup.h"
#include "Utils.h"
#include "colors.h"

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

    unsigned int response{ 0 };

    while (!Utils::AskResponse("Select device to target (Green are known devices)", 1, static_cast<unsigned int>(devices.size()), response)) {};

    std::cout << "Selected Target device:\n" << devices[response - 1].getTargetInfo() << std::endl;

    return ps;
}
