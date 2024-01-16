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
#include "MacroHandler.h"
#include <format>

#pragma comment(lib, "hid.lib")
#pragma comment(lib, "setupapi.lib")

using namespace std;

bool StartMonitor(WORD targetVID, WORD targetPID)
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
    MacroHandler                    mh;

    char* manufacturerName{};
    char* productName{};
    short vid, pid, usagepage, usagecode;

    // Find all HID devices attached to the system.
    if (!FindKnownHidDevices(&pDevice, &numberDevices))
    {
        cerr << "No HID devices found." << endl;
        return false;
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

            // Allocate space for wide-char Manufacturer string
            wchar_t* wmanufacturerName{};
            wchar_t* wproductName{};
            try
            {
                wmanufacturerName = new wchar_t[256] {};
                wproductName = new wchar_t[256] {};
            }
            catch (const bad_alloc&)
            {
                return false;
            }

            if (HidD_GetManufacturerString(pDevice->HidDevice, wmanufacturerName, 256) && HidD_GetProductString(pDevice->HidDevice, wproductName, 256))
            {
                int mnsizeRequired = WideCharToMultiByte(CP_UTF8, 0, wmanufacturerName, -1, nullptr, 0, NULL, NULL);
                int pnsizeRequired = WideCharToMultiByte(CP_UTF8, 0, wproductName, -1, nullptr, 0, NULL, NULL);
                try
                {
                    manufacturerName = new char[mnsizeRequired] {};
                    productName = new char[pnsizeRequired] {};
                }
                catch (const bad_alloc&)
                {
                    return false;
                }
                WideCharToMultiByte(CP_UTF8, 0, wmanufacturerName, -1, manufacturerName, mnsizeRequired, NULL, NULL);
                WideCharToMultiByte(CP_UTF8, 0, wproductName, -1, productName, pnsizeRequired, NULL, NULL);
                delete[] wmanufacturerName;             // no longer necessary.
                delete[] wproductName;
            }

            vid = pDevice->Attributes.VendorID;
            pid = pDevice->Attributes.ProductID;
            usagepage = pDevice->Caps.UsagePage;
            usagecode = pDevice->Caps.Usage;

            // Try to allocate memory for storing the Device Path
            try
            {
                targetDevicePath = new char[iDevicePathSize] {};
            }
            catch (const bad_alloc&)
            {
                cerr << "Unable to allocate memory for device path." << endl;
                return false;
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
        cerr << "Target device could not be located!" << endl;
        return false;
    }

#ifdef _DEBUG
    cout << format("\nTarget Device located:\n{} {}\nVID:        {:#06x}\nPID:        {:#06x}\nUsagePage:  {:#04x}\nUsage:      {:#04x}\n",
                   manufacturerName, productName, vid, pid, usagepage, usagecode) << endl;
#endif

    delete[] manufacturerName;          // no longer needed. Can be deleted.
    delete[] productName;

    // Open target device for asynchronous reading
    bool openForAsync = OpenHidDevice(targetDevicePath, true, false, true, false, &targetDevice);

    delete[] targetDevicePath;

    if (!openForAsync)
    {
        cerr << "Unable to open target HID device for async read" << endl;
        return false;
    }

    cout << "Starting monitor" << endl;

    completionEvent = CreateEvent(nullptr, false, false, nullptr);

    if (completionEvent == nullptr)
    {
        return false;
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

        // If the key that is being pressed is between our min and max MACROs, process the macro key.
        if (usage >= MACROA && usage <= MACROD)
        {
#ifdef _DEBUG
            cout << "Read key: 0x" << hex << usage << " Macro " << static_cast<char>(usage - 0xb) << endl;
#endif
            mh.Process(usage);
        }
    } while (readResult);

    return true;
}
