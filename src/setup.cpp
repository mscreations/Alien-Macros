#include <iostream>
#include <iomanip>
#include <format>
#include <memory>
#include "setup.h"
#include "Utils.h"
#include "MacroAction.h"
#include "colors.h"

MacroAction Setup::SelectMacroAction(const std::string& description)
{
    std::cout << Colors::ClearScreen << std::format("Select Macro Action for {}:\n\n", description);
    std::cout << "1. String\n";
    std::cout << "2. Single character (char)\n";
    std::cout << "3. Virtual Key / Trigger AutoHotKey\n\n";

    unsigned int response{ 0 };
    while (!Utils::AskResponse("Enter selection", 1, 3, response)) {}

    std::string str{};
    char ch{};

    switch (response)
    {
        case 1:
            while (!Utils::AskResponse("Please enter string to be typed when Macro key pressed", str)) {};
            return MacroAction(str, description);
        case 2:
            while (!Utils::AskResponse("Please enter a single printable character to be typed when Macro key pressed", ch)) {};
            return MacroAction(ch, description);
        case 3:
            // TODO: Not implemented yet
            return MacroAction("Not implemented yet", description);
    }

    return MacroAction(description);
}

void Setup::OutputDeviceList(const HidDevices& devices)
{
    for (int i = 1; const auto & dev : devices.getDevices())
    {
        bool known = false;
        if (dev->IsKnownTarget())
        {
            known = true;
        }
        std::cout << (known ? Colors::Green : "") << std::setw(3) << i++ << ": " << *dev.get() << (known ? Colors::Reset : "");
    }
}

HidDevicePtr Setup::ChooseTarget()
{
    HidDevices devices{};
    devices.FindAllHidDevices(true);

    std::cout << Colors::ClearScreen << Colors::Green << "\n\nConnected HID Devices:\n" << Colors::Reset;
    OutputDeviceList(devices);

    std::cout << std::endl;

    unsigned int response{ 0 };

    while (!Utils::AskResponse("Select device to target (Green are known devices)", 1, static_cast<unsigned int>(devices.size()), response)) {};

    return std::move(devices[response - 1]);
}

bool Setup::VerifyTarget(HidDevicePtr& target)
{
    target->Open(true, false, true);
    std::cout << Colors::ClearScreen << Colors::Green << "Press a macro key to verify correct device being received (Control-C to cancel): " << Colors::Reset;

    if (!target->ReadAsync(1))
    {
        // User pressed Control-C or something else went wrong.
        return false;
    }
    USAGE pressedKey = target->getKeyPress();
    if (pressedKey != 0x00)
    {
        std::cout << "0x" << std::hex << pressedKey << std::endl;
        std::cout << "Read successful!" << std::endl;
        return true;
    }
    return false;

}

std::unique_ptr<ProgSettings> Setup::invokeSetup()
{
    // ****1. Search all HID devices
    // ****     a. Locate all devices
    // ****     b. If a known device, recommend it
    // ****     c. Wait until user makes valid response
    // ****     d. Return a pointer to specific HidDevice
    // ****2. Have user verify macro keys can be read from HidDevice
    // ****     a. Open HidDevice for read *** Going to have to implement Async read so we are not thread locked if device is incorrect ******
    // ****     b. Wait for user to press Macro key to verify correct device.
    // ****3. Ask user for # of macro keys
    // ****4. For each key:
    // ****     a. Ask user to press key
    // ****     b. Ask user for description
    //      c. Ask user what action should be completed
    //      d. Ask user what payload should be used.
    // 5. After all settings captured, display summary and ask user to verify
    // 6. Save all settings to registry
    // 7. Ask user if program should be installed to auto-start on Windows startup
    //      a. If yes, ask if we should move to a different directory
    //      b. Create scheduled task to start program in saved location
    // 8. If any macro keys are set to Virtual Keys, ask if user is using an autohotkey script that needs to run with this.
    //      a. If so, Set parameter in registry with script name so it can be started on this program startup.

    auto ps = std::make_unique<ProgSettings>(true);
    do
    {
        ps->device = ChooseTarget();
    } while (!VerifyTarget(ps->device));

    ps->target = ps->device->getTargetInfo();

    unsigned int macrokeys{ 0 };
    while (!Utils::AskResponse("How many macro keys does your keyboard have?", macrokeys)) {}

    ps->device->Open(true);
    for (unsigned int i = 0; i < macrokeys; i++)
    {
        USAGE pressedKey{ 0 };
        std::cout << Colors::ClearScreen << Colors::Green << "Please press macro key #" << i + 1 << Colors::Reset << std::endl;
        bool valid = false;
        while (!valid)
        {
            ps->device->Read();
            pressedKey = ps->device->getKeyPress();
            if (pressedKey != 0x00) { valid = true; }
            if (ps->macrolist.find(pressedKey) != ps->macrolist.end())
            {
                std::cout << Colors::Red << std::format("Read key already assigned: {:#04x} Assigned action: ", pressedKey) << ps->macrolist[pressedKey] << Colors::Reset << std::endl;
                std::cout << "Please try again" << std::endl;
                valid = false;
            }
        };
        std::cout << "Successfully read key with code: 0x" << std::hex << pressedKey << std::endl;

        std::string description{};
        while (!Utils::AskResponse("Please provide a description for key", description))
        {
            std::cout << Colors::CursorPreviousLine;
        }

        ps->macrolist[pressedKey] = SelectMacroAction(description);


    }

    //std::cout << "Selected Target device:\n" << devices[response - 1].getTargetInfo() << std::endl;

    return ps;
}
