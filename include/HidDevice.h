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
#include "TargetDevice.h"

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

    struct _ButtonData
    {
        ULONG                       UsageMin;       // Variables to track the usage minimum and max
        ULONG                       UsageMax;       // If equal, then only a single usage
        ULONG                       MaxUsageLength; // Usages buffer length.
        std::vector<USAGE>          Usages;         // list of usages (buttons ``down'' on the device.

        _ButtonData() : UsageMin{ 0 }, UsageMax{ 0 }, MaxUsageLength{ 0 }, Usages{ std::vector<USAGE>() } {}
    } ButtonData;
    struct _ValueData
    {
        USAGE                       Usage;          // The usage describing this value
        USHORT                      Reserved;
        ULONG                       Value;
        LONG                        ScaledValue;
        _ValueData() : Usage{ 0 }, Reserved{ 0 }, Value{ 0 }, ScaledValue{ 0 } {}
    } ValueData;

    _HID_DATA() : IsButtonData(true), Reserved(0), UsagePage(0), Status(0), ReportID(0), IsDataSet(false), ButtonData{}, ValueData{} {}
    ~_HID_DATA() { ButtonData.Usages.clear(); }
} HID_DATA;

using HidDataPtr = std::unique_ptr<HID_DATA[]>;
using HidButtonCaps = std::unique_ptr<HIDP_BUTTON_CAPS[]>;
using HidValueCaps = std::unique_ptr<HIDP_VALUE_CAPS[]>;
using charBufferPtr = std::unique_ptr<char[]>;

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

    charBufferPtr                           InputReportBuffer{};
    unsigned long                           InputDataLength{ 0 };
    HidDataPtr                              InputData{};
    HidButtonCaps                           InputButtonCaps{};
    HidValueCaps                            InputValueCaps{};

    charBufferPtr                           OutputReportBuffer{};
    unsigned long                           OutputDataLength{ 0 };
    HidDataPtr                              OutputData{};
    HidButtonCaps                           OutputButtonCaps{};
    HidValueCaps                            OutputValueCaps{};

    charBufferPtr                           FeatureReportBuffer{};
    unsigned long                           FeatureDataLength{ 0 };
    HidDataPtr                              FeatureData{};
    HidButtonCaps                           FeatureButtonCaps{};
    HidValueCaps                            FeatureValueCaps{};


    std::string LoadHidString(const std::function<BOOLEAN(HANDLE, PVOID, ULONG)>) const;

    static void SetHidData(HidDataPtr& ptr, const unsigned long offset, const USAGE up, const USAGE usage, const unsigned long rid);

    static bool UnpackReport(charBufferPtr& ReportBuffer,
                             unsigned short ReportBufferLength,
                             HIDP_REPORT_TYPE ReportType,
                             HidDataPtr& Data,
                             unsigned long DataLength,
                             std::unique_ptr<PHIDP_PREPARSED_DATA>& Ppd);
public:
    explicit HidDevice(const std::string& DevicePath);
    ~HidDevice();
    bool IsOpen() const;
    void Close();
    bool Read();
    bool Open(bool HasReadAccess = false, bool HasWriteAccess = false, bool IsOverlapped = false, bool IsExclusive = false);
    bool FillDevice();
    TargetDevice getTargetInfo() const;
    bool IsTarget(int vid, int pid, int usagepage, int usagecode);
    USAGE getKeyPress() const;

    friend std::ostream& operator<<(std::ostream& strm, const HidDevice& hd);
};

using HidDevicePtr = std::unique_ptr<HidDevice>;

class HidDevices
{
    std::vector<HidDevicePtr> devices;

public:
    HidDevices();
    bool FindAllHidDevices(bool CloseAllDevices = false);
    std::vector<HidDevicePtr>& getDevices();
    const std::vector<HidDevicePtr>& getDevices() const;

    friend std::ostream& operator<<(std::ostream& strm, const HidDevices& hds);
};
