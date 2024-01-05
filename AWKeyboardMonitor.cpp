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

#pragma comment(lib, "hid.lib")
#pragma comment(lib, "setupapi.lib")

DWORD StartMonitor(WORD targetVID, WORD targetPID)
{
    static READ_THREAD_CONTEXT      readContext;
    static HANDLE                   readThread;
    static HID_DEVICE               asyncDevice;
    static HID_DEVICE               targetDevice;

    PHID_DEVICE                     pDevice;
    PHID_DEVICE                     tempDeviceList = NULL;
    ULONG                           numberDevices;
    DWORD                           threadID;

    readThread = NULL;

    if (!FindKnownHidDevices(&tempDeviceList, &numberDevices))
    {
        std::cerr << "No HID devices found." << std::endl;
        return 0;
    }

    pDevice = tempDeviceList;
    for (int iIndex = 0; (ULONG)iIndex < numberDevices; iIndex++) {
        if (pDevice->Attributes.VendorID == targetVID &&
            pDevice->Attributes.ProductID == targetPID &&
            pDevice->Caps.UsagePage == AW_USAGEPAGE &&
            pDevice->Caps.Usage == AW_USAGE)
        {
            iIndex = numberDevices;             // abort for loop now that we found the device we want.
            targetDevice = *pDevice;
        }
        else {
            pDevice++;
        }
    }
    free(tempDeviceList);
    tempDeviceList = NULL;

    std::cout << "Target Device located: " << targetDevice.DevicePath << std::endl;

    BOOL openForAsync = OpenHidDevice(targetDevice.DevicePath, TRUE, FALSE, TRUE, FALSE, &asyncDevice);

    if (!openForAsync) {
        std::cerr << "Unable to open target HID device for async read" << std::endl;
        return 0;
    }

    readContext.HidDevice = &asyncDevice;
    readContext.TerminateThread = FALSE;

    std::cout << "Starting monitor" << std::endl;

    readThread = CreateThread(NULL,
        0,
        ReadThreadProc,
        &readContext,
        0,
        &threadID);

    while(true) { }

    return 0;
}

DWORD WINAPI ReadThreadProc(LPVOID lParam) {
    HANDLE                  completionEvent;
    BOOL                    readResult;
    DWORD                   waitStatus;
    OVERLAPPED              overlap;
    DWORD                   bytesTransferred;
    PREAD_THREAD_CONTEXT    Context = (PREAD_THREAD_CONTEXT)lParam;
    PHID_DEVICE             pDevice = Context->HidDevice;

    completionEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

    if (NULL == completionEvent) {
        ExitThread(0);
        return 0;
    }

    readResult = TRUE;

    do
    {
        readResult = ReadOverlapped(Context->HidDevice, completionEvent, &overlap);

        if (!readResult) {
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

            if (usage >= MACROA && usage <= MACROD) {

                std::cout << "Read key: 0x" << std::hex << usage;
                switch (usage) {
                case MACROA: std::cout << " Macro A" << std::endl; break;
                case MACROB: std::cout << " Macro B" << std::endl; break;
                case MACROC: std::cout << " Macro C" << std::endl; break;
                case MACROD: std::cout << " Macro D" << std::endl; break;
                }

                INPUT inputs[2] = {};
                ZeroMemory(inputs, sizeof(inputs));

                inputs[0].type = INPUT_KEYBOARD;
                inputs[0].ki.wVk = usage + 0x30;                            // Maps the macro keys to F13-F16

                inputs[1].type = INPUT_KEYBOARD;
                inputs[1].ki.wVk = usage + 0x30;
                inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;

                SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
            }
        }
    } while (readResult &&
        !Context->TerminateThread);

    ExitThread(0);
    return 0;
}
