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

#include "Alien-Macros.h"
#include <strsafe.h>

#pragma comment(lib, "hid.lib")
#pragma comment(lib, "setupapi.lib")

DWORD StartMonitor(WORD targetVID, WORD targetPID)
{
    static READ_THREAD_CONTEXT      readContext;
    static HANDLE                   readThread;
    static HID_DEVICE               targetDevice;

    PCHAR                           targetDevicePath = nullptr;
    PHID_DEVICE                     pDevice = nullptr;
    PHID_DEVICE                     tempDeviceList = nullptr;
    ULONG                           numberDevices;
    DWORD                           threadID;

    readThread = nullptr;

    if (!FindKnownHidDevices(&tempDeviceList, &numberDevices))
    {
        std::cerr << "No HID devices found." << std::endl;
        return -1;
    }

    pDevice = tempDeviceList;
    for (int iIndex = 0; (ULONG)iIndex < numberDevices; iIndex++)
    {
        if (pDevice->Attributes.VendorID == targetVID &&
            pDevice->Attributes.ProductID == targetPID &&
            pDevice->Caps.UsagePage == AW_USAGEPAGE &&
            pDevice->Caps.Usage == AW_USAGE)
        {
            iIndex = numberDevices;             // abort for loop now that we found the device we want.
            int iDevicePathSize = static_cast<int>(strnlen(pDevice->DevicePath, MAX_PATH) + 1);
            targetDevicePath = new char[iDevicePathSize];
            if (targetDevicePath == nullptr) 
            { 
                std::cerr << "Unable to allocate memory for device path." << std::endl;
                return -1; 
            }
            StringCbCopyA(targetDevicePath, iDevicePathSize, pDevice->DevicePath);
            pDevice -= iIndex;
        }
        else
        {
            pDevice++;
        }
    }
    free(tempDeviceList);
    tempDeviceList = nullptr;
    pDevice = nullptr;

    if (targetDevicePath == nullptr)
    {
        std::cerr << "Target device could not be located!" << std::endl;
        return -1;
    }

    std::cout << "Target Device located: " << targetDevicePath << std::endl;

    BOOL openForAsync = OpenHidDevice(targetDevicePath, TRUE, FALSE, TRUE, FALSE, &targetDevice);

    delete[] targetDevicePath;

    if (!openForAsync)
    {
        std::cerr << "Unable to open target HID device for async read" << std::endl;
        return -1;
    }

    readContext.HidDevice = &targetDevice;
    readContext.TerminateThread = FALSE;

    std::cout << "Starting monitor" << std::endl;

    readThread = CreateThread(nullptr,
                              0,
                              ReadThreadProc,
                              &readContext,
                              0,
                              &threadID);

    while (true) {}

    return 0;
}

DWORD WINAPI ReadThreadProc(LPVOID lParam)
{
    HANDLE                  completionEvent;
    BOOL                    readResult;
    DWORD                   waitStatus;
    OVERLAPPED              overlap;
    DWORD                   bytesTransferred;
    PREAD_THREAD_CONTEXT    Context = static_cast<PREAD_THREAD_CONTEXT>(lParam);
    PHID_DEVICE             pDevice = Context->HidDevice;

    completionEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);

    if (completionEvent == nullptr)
    {
        ExitThread(0);
        return 0;
    }

    readResult = TRUE;

    do
    {
        readResult = ReadOverlapped(Context->HidDevice, completionEvent, &overlap);

        if (!readResult)
        {
            break;
        }
        while (!Context->TerminateThread)
        {
            waitStatus = WaitForSingleObject(completionEvent, READ_THREAD_TIMEOUT);

            if (WAIT_OBJECT_0 == waitStatus)
            {
                readResult = GetOverlappedResult(Context->HidDevice->HidDevice, &overlap, &bytesTransferred, TRUE);
                break;
            }
        }
        if (!Context->TerminateThread)
        {
            UnpackReport(pDevice->InputReportBuffer,
                         pDevice->Caps.InputReportByteLength,
                         HidP_Input,
                         pDevice->InputData,
                         pDevice->InputDataLength,
                         pDevice->Ppd);

            USAGE usage = *pDevice->InputData->ButtonData.Usages;

            if (usage >= MACROA && usage <= MACROD)
            {
                HandleMacroKey(usage);
            }
        }
    } while (readResult &&
             !Context->TerminateThread);

    ExitThread(0);
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