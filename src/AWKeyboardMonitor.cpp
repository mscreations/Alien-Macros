/*
 *  Alien-Macro
 *  Receives Macro keypresses and translates to a keycode that AutoHotKey can understand.
 *  Copyright (C) 2023 mscreations
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 *  CURRENT STATUS: Proof of Concept. Still needs a lot of refinement and should not be used
 *      in normal day to day use.
 *
 */

#include <iostream>
#include <wtypes.h>
#include <strsafe.h>
#include "hid.h"
#include <AWKeyboardMonitor.h>

#pragma comment(lib, "hid.lib")
#pragma comment(lib, "setupapi.lib")

DWORD StartMonitor(WORD targetVID, WORD targetPID)
{
    static HID_DEVICE               targetDevice;
    HANDLE                          completionEvent;
    bool                            readResult;
    DWORD                           waitStatus;
    OVERLAPPED                      overlap;
    DWORD                           bytesTransferred;
    PCHAR                           targetDevicePath = nullptr;
    PHID_DEVICE                     pDevice = nullptr;
    ULONG                           numberDevices;

    if (!FindKnownHidDevices(&pDevice, &numberDevices))
    {
        std::cerr << "No HID devices found." << std::endl;
        return -1;
    }

    for (ULONG iIndex = 0; iIndex < numberDevices; iIndex++, pDevice++)
    {
        // If currently pointed to device is our target device, copy the DevicePath into a buffer to use later
        if (pDevice->Attributes.VendorID == targetVID &&
            pDevice->Attributes.ProductID == targetPID &&
            pDevice->Caps.UsagePage == AW_USAGEPAGE &&
            pDevice->Caps.Usage == AW_USAGE)
        {
            int iDevicePathSize = static_cast<int>(strnlen(pDevice->DevicePath, MAX_PATH) + 1);
            // Try to allocate memory for storing the Device Path
            try
            {
                targetDevicePath = new char[iDevicePathSize];
                std::memset(targetDevicePath, 0, iDevicePathSize);
            }
            catch (const std::bad_alloc&)
            {
                std::cerr << "Unable to allocate memory for device path." << std::endl;
                return -1;
            }
            StringCbCopyA(targetDevicePath, iDevicePathSize, pDevice->DevicePath);
            pDevice -= iIndex;          // Move pDevice pointer back to the beginning of the list again in preparation for the free statement
            break;
        }
    }
    delete[] pDevice;
    pDevice = nullptr;

    if (targetDevicePath == nullptr)
    {
        std::cerr << "Target device could not be located!" << std::endl;
        return -1;
    }

#ifdef _DEBUG
    std::cout << "Target Device located: " << targetDevicePath << std::endl;
#endif

    // Open target device for asynchronous reading
    bool openForAsync = OpenHidDevice(targetDevicePath, true, false, true, false, &targetDevice);

    delete[] targetDevicePath;

    if (!openForAsync)
    {
        std::cerr << "Unable to open target HID device for async read" << std::endl;
        return -1;
    }

    std::cout << "Starting monitor" << std::endl;

    completionEvent = CreateEvent(nullptr, false, false, nullptr);

    if (completionEvent == nullptr)
    {
        return -1;
    }

    readResult = true;

    // Begin monitoring loop. This likely could be made more efficient. Is is mostly a copy of Microsoft's hclient sample
    do
    {
        readResult = ReadOverlapped(&targetDevice, completionEvent, &overlap);

        if (!readResult)
        {
            break;
        }
        while (true)
        {
            waitStatus = WaitForSingleObject(completionEvent, READ_THREAD_TIMEOUT);

            if (waitStatus == WAIT_OBJECT_0)
            {
                readResult = GetOverlappedResult(targetDevice.HidDevice, &overlap, &bytesTransferred, true);
                break;
            }
        }
        UnpackReport(targetDevice.InputReportBuffer,
                     targetDevice.Caps.InputReportByteLength,
                     HidP_Input,
                     targetDevice.InputData,
                     targetDevice.InputDataLength,
                     targetDevice.Ppd);

        USAGE usage = *targetDevice.InputData->ButtonData.Usages;

        if (usage >= MACROA && usage <= MACROD)
        {
            HandleMacroKey(usage);
        }
    } while (readResult);

    return 0;
}

void HandleMacroKey(USAGE macroKey)
{
    std::cout << "Read key: 0x" << std::hex << macroKey << " Macro " << (char)(macroKey - 0xb) << std::endl;

    INPUT inputs[2] = {};
    ZeroMemory(inputs, sizeof(inputs));

    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = macroKey + 0x30;                            // Maps the macro keys to F13-F16

    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.wVk = macroKey + 0x30;
    inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;

    SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
}