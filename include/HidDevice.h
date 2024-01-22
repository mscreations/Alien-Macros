/*
 *  Alien-Macros
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
 */

#pragma once

#include <memory>
#include <string>
#include <iostream>
#include <vector>
#include <format>
#include <functional>
#include <Windows.h>
#include <hidsdi.h>
#include <SetupAPI.h>

#pragma comment(lib, "hid.lib")
#pragma comment(lib, "setupapi.lib")

typedef struct _HID_DATA
{
    bool                                IsButtonData;
    UCHAR                               Reserved;
    USAGE                               UsagePage;      // The usage page for which we are looking.
    ULONG                               Status;         // The last status returned from the accessor function
    ULONG                               ReportID;       // ReportID for this given data structure
    bool                                IsDataSet;      // Variable to track whether a given data structure
    //  has already been added to a report structure

    union
    {
        struct
        {
            ULONG                       UsageMin;       // Variables to track the usage minimum and max
            ULONG                       UsageMax;       // If equal, then only a single usage
            ULONG                       MaxUsageLength; // Usages buffer length.
            PUSAGE                      Usages;         // list of usages (buttons ``down'' on the device.
        } ButtonData;
        struct
        {
            USAGE                       Usage;          // The usage describing this value
            USHORT                      Reserved;
            ULONG                       Value;
            LONG                        ScaledValue;
        } ValueData;
    };

    _HID_DATA()
        : IsButtonData(false), Reserved(0), UsagePage(0), Status(0), ReportID(0), IsDataSet(false)
    {
        ButtonData.UsageMin = ButtonData.UsageMax = ButtonData.MaxUsageLength = 0;
        ButtonData.Usages = nullptr;
        ValueData.Usage = 0;
        ValueData.Value = 0;
        ValueData.Reserved = 0;
        ValueData.ScaledValue = 0;
    }

    ~_HID_DATA()
    {
        if (ButtonData.Usages != nullptr)
        {
            delete[] ButtonData.Usages;
        }
    }
} HID_DATA;

class HidDevice
{
    std::string                             DevicePath;
    std::string                             ManufacturerString;
    std::string                             ProductString;
    HANDLE                                  device{};
    bool                                    OpenedForRead{ false };
    bool                                    OpenedForWrite{ false };
    bool                                    OpenedOverlapped{ false };
    bool                                    OpenedExclusive{ false };

    std::unique_ptr<PHIDP_PREPARSED_DATA>   Ppd{};
    std::unique_ptr<HIDP_CAPS>              Caps{};
    std::unique_ptr<HIDD_ATTRIBUTES>        Attributes{};

    std::unique_ptr<char[]>                 InputReportBuffer{};
    unsigned long                           InputDataLength{ 0 };
    std::unique_ptr<HID_DATA[]>             InputData{};
    std::unique_ptr<HIDP_BUTTON_CAPS[]>     InputButtonCaps{};
    std::unique_ptr<HIDP_VALUE_CAPS[]>      InputValueCaps{};

    std::unique_ptr<char[]>                 OutputReportBuffer{};
    unsigned long                           OutputDataLength{ 0 };
    std::unique_ptr<HID_DATA[]>             OutputData{};
    std::unique_ptr<HIDP_BUTTON_CAPS[]>     OutputButtonCaps{};
    std::unique_ptr<HIDP_VALUE_CAPS[]>      OutputValueCaps{};

    std::unique_ptr<char[]>                 FeatureReportBuffer{};
    unsigned long                           FeatureDataLength{ 0 };
    std::unique_ptr<HID_DATA[]>             FeatureData{};
    std::unique_ptr<HIDP_BUTTON_CAPS[]>     FeatureButtonCaps{};
    std::unique_ptr<HIDP_VALUE_CAPS[]>      FeatureValueCaps{};


    std::string LoadHidString(std::function<BOOLEAN(HANDLE, PVOID, ULONG)>) const;

    static void SetHidData(std::unique_ptr<HID_DATA[]>& ptr, unsigned long offset, USAGE up, USAGE usage, unsigned long rid);

    static bool UnpackReport(std::unique_ptr<char[]>& ReportBuffer,
                             unsigned short ReportBufferLength,
                             HIDP_REPORT_TYPE ReportType,
                             std::unique_ptr<HID_DATA[]>& Data,
                             unsigned long DataLength,
                             std::unique_ptr<PHIDP_PREPARSED_DATA>& Ppd);
    static bool PackReport();

public:
    HidDevice(std::string DevicePath);
    ~HidDevice();
    bool IsOpen() const;
    void Close();
    bool Read();
    bool ReadOverlapped();
    bool Write();
    bool Open(bool HasReadAccess = false, bool HasWriteAccess = false, bool IsOverlapped = false, bool IsExclusive = false);
    bool FillDevice();
    bool IsTarget(int vid, int pid, int usagepage, int usagecode);
    USAGE getKeyPress();

    friend std::ostream& operator<<(std::ostream& strm, const HidDevice& hd);
};

using HidDevicePtr = std::unique_ptr<HidDevice>;

class HidDevices
{
    std::vector<HidDevicePtr> devices;

public:
    HidDevices();
    bool FindAllHidDevices();
    std::vector<HidDevicePtr>& getDevices();
    const std::vector<HidDevicePtr>& getDevices() const;

    friend std::ostream& operator<<(std::ostream& strm, const HidDevices& hds);
};
