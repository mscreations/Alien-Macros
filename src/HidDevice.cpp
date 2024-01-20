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

#include "HidDevice.h"

bool HidDevice::UnpackReport()
{
    return false;
}

bool HidDevice::PackReport()
{
    return false;
}

void HidDevice::SetHidData(std::unique_ptr<HID_DATA[]>& ptr, unsigned long offset, USAGE up, USAGE usage, unsigned long rid)
{
    (ptr.get()[offset]).IsButtonData = false;
    (ptr.get()[offset]).Status = HIDP_STATUS_SUCCESS;
    (ptr.get()[offset]).UsagePage = up;
    (ptr.get()[offset]).ValueData.Usage = usage;
    (ptr.get()[offset]).ReportID = rid;
}

HidDevice::HidDevice(std::string DevicePath)
{
    this->DevicePath = DevicePath;
    this->device = INVALID_HANDLE_VALUE;
    this->Ppd = std::make_unique<PHIDP_PREPARSED_DATA>();
    this->Attributes = std::make_unique<HIDD_ATTRIBUTES>();
    this->Caps = std::make_unique<HIDP_CAPS>();
}

HidDevice::~HidDevice()
{
    if (IsOpen())
    {
        CloseHandle(device);
        device = INVALID_HANDLE_VALUE;
    }
}

void HidDevice::Close()
{
}

bool HidDevice::IsOpen()
{
    return device != INVALID_HANDLE_VALUE;
}

bool HidDevice::Read()
{
    return false;
}

bool HidDevice::ReadOverlapped()
{
    return false;
}

bool HidDevice::Write()
{
    return false;
}

bool HidDevice::Open(bool HasReadAccess, bool HasWriteAccess, bool IsOverlapped, bool IsExclusive)
{
    unsigned long accessFlags{ 0 }, sharingFlags{ 0 };

    if (DevicePath.empty()) { return false; }

    if (HasReadAccess)
    {
        accessFlags |= GENERIC_READ;
    }

    if (HasWriteAccess)
    {
        accessFlags |= GENERIC_WRITE;
    }

    if (!IsExclusive)
    {
        sharingFlags = FILE_SHARE_READ | FILE_SHARE_WRITE;
    }

    device = CreateFileA(DevicePath.c_str(),
                         accessFlags,
                         sharingFlags,
                         nullptr,
                         OPEN_EXISTING,
                         0,
                         nullptr);

    if (device == INVALID_HANDLE_VALUE) { return false; }

    OpenedForRead = HasReadAccess;
    OpenedForWrite = HasWriteAccess;
    OpenedOverlapped = IsOverlapped;
    OpenedExclusive = IsExclusive;

    if (!HidD_GetPreparsedData(device, Ppd.get()) ||
        !HidD_GetAttributes(device, Attributes.get()) ||
        !HidP_GetCaps(*Ppd.get(), Caps.get()) ||
        !FillDevice())
    {
        return false;
    }
    return false;
}

bool HidDevice::FillDevice()
{
    unsigned short numCaps{ 0 };
    unsigned long numValues{ 0 };

    // Load Manufacturer String
    {
        auto wMFString = std::make_unique<wchar_t[]>(256);
        if (HidD_GetManufacturerString(device, wMFString.get(), 256))
        {
            int sizeRequired = WideCharToMultiByte(CP_UTF8, 0, wMFString.get(), -1, nullptr, 0, NULL, NULL);
            auto charMFString = std::make_unique<char[]>(sizeRequired);
            WideCharToMultiByte(CP_UTF8, 0, wMFString.get(), -1, charMFString.get(), sizeRequired, NULL, NULL);
            ManufacturerString = std::string(charMFString.get());
        }
    }
    // Load Product String
    {
        auto wPString = std::make_unique<wchar_t[]>(256);
        if (HidD_GetProductString(device, wPString.get(), 256))
        {
            int sizeRequired = WideCharToMultiByte(CP_UTF8, 0, wPString.get(), -1, nullptr, 0, NULL, NULL);
            auto charPString = std::make_unique<char[]>(sizeRequired);
            WideCharToMultiByte(CP_UTF8, 0, wPString.get(), -1, charPString.get(), sizeRequired, NULL, NULL);
            ProductString = std::string(charPString.get());
        }
    }

    // Allocate memory for input report
    if (Caps->InputReportByteLength > 0)
    {
        InputReportBuffer = std::make_unique<char[]>(Caps->InputReportByteLength);
    }

    // Have the HidP_X functions fill in the capability structure arrays
    numCaps = Caps->NumberInputButtonCaps;

    if (numCaps > 0)
    {
        InputButtonCaps = std::make_unique<HIDP_BUTTON_CAPS[]>(numCaps);
        if (HIDP_STATUS_SUCCESS != (HidP_GetButtonCaps(HidP_Input,
                                                       InputButtonCaps.get(),
                                                       &numCaps,
                                                       *Ppd.get())))
        {
            return false;
        }
    }

    numCaps = Caps->NumberInputValueCaps;

    if (numCaps > 0)
    {
        InputValueCaps = std::make_unique<HIDP_VALUE_CAPS[]>(numCaps);
        if (HIDP_STATUS_SUCCESS != (HidP_GetValueCaps(HidP_Input,
                                                      InputValueCaps.get(),
                                                      &numCaps,
                                                      *Ppd.get())))
        {
            return false;
        }
    }

    // Depending on the device, some value caps structures may represent more
    // than one value.  (A range).  In the interest of being verbose, over
    // efficient, we will expand these so that we have one and only one
    // struct _HID_DATA for each value.
    //
    // To do this we need to count up the total number of values are listed
    // in the value caps structure.  For each element in the array we test
    // for range if it is a range then UsageMax and UsageMin describe the
    // usages for this range INCLUSIVE.

    for (int i = 0; i < Caps->NumberInputValueCaps; i++)
    {
        if ((InputValueCaps.get()[i]).IsRange)
        {
            numValues += (InputValueCaps.get()[i]).Range.UsageMax - (InputValueCaps.get()[i]).Range.UsageMin + 1;
            if ((InputValueCaps.get()[i]).Range.UsageMin > (InputValueCaps.get()[i]).Range.UsageMax)
            {
                return false;
            }
        }
        else
        {
            numValues++;
        }
    }

    // Allocate a buffer to hold the struct _HID_DATA structures.
    // One element for each set of buttons, and one element for each value
    // found.

    InputDataLength = Caps->NumberInputButtonCaps + numValues;

    InputData = std::make_unique<HID_DATA[]>(InputDataLength);

    // Fill in the button data

    unsigned long dataIdx{ 0 };
    for (int i = 0; i < Caps->NumberInputButtonCaps; i++, dataIdx++)
    {
        (InputData.get()[dataIdx]).IsButtonData = true;
        (InputData.get()[dataIdx]).Status = HIDP_STATUS_SUCCESS;
        (InputData.get()[dataIdx]).UsagePage = (InputButtonCaps.get()[dataIdx]).UsagePage;
        if ((InputButtonCaps.get()[dataIdx]).IsRange)
        {
            (InputData.get()[dataIdx]).ButtonData.UsageMin = (InputButtonCaps.get()[dataIdx]).Range.UsageMin;
            (InputData.get()[dataIdx]).ButtonData.UsageMax = (InputButtonCaps.get()[dataIdx]).Range.UsageMax;
        }
        else
        {
            (InputData.get()[dataIdx]).ButtonData.UsageMin = (InputData.get()[dataIdx]).ButtonData.UsageMax = (InputButtonCaps.get()[dataIdx]).NotRange.Usage;
        }

        (InputData.get()[dataIdx]).ButtonData.MaxUsageLength = HidP_MaxUsageListLength(HidP_Input,
                                                                                 (InputButtonCaps.get()[dataIdx]).UsagePage,
                                                                                 *Ppd.get());

        (InputData.get()[dataIdx]).ReportID = (InputButtonCaps.get()[dataIdx]).ReportID;
    }

    // Fill in the value data

    for (int i = 0; i < Caps->NumberInputValueCaps; i++)
    {
        if ((InputValueCaps.get()[i]).IsRange)
        {
            for (USAGE usage = (InputValueCaps.get()[i]).Range.UsageMin;
                 usage <= (InputValueCaps.get()[i]).Range.UsageMax;
                 usage++)
            {
                if (dataIdx >= (InputDataLength)) { return false; }
                HidDevice::SetHidData(InputData, dataIdx, (InputValueCaps.get()[i]).UsagePage,
                                      usage, (InputValueCaps.get()[i]).ReportID);
            }
        }
        else
        {
            HidDevice::SetHidData(InputData, dataIdx, (InputValueCaps.get()[i]).UsagePage,
                                  (InputValueCaps.get()[i]).NotRange.Usage, (InputValueCaps.get()[i]).ReportID);
        }
    }

    // Setup Output Data Buffers

    OutputReportBuffer = std::make_unique<char[]>(Caps->OutputReportByteLength);

    numCaps = Caps->NumberOutputButtonCaps;
    OutputButtonCaps = std::make_unique<HIDP_BUTTON_CAPS[]>(numCaps);
    if (numCaps > 0)
    {
        if (HIDP_STATUS_SUCCESS != (HidP_GetButtonCaps(HidP_Output,
                                                       OutputButtonCaps.get(),
                                                       &numCaps,
                                                       *Ppd.get())))
        {
            return false;
        }
    }

    numCaps = Caps->NumberOutputValueCaps;
    OutputValueCaps = std::make_unique<HIDP_VALUE_CAPS[]>(numCaps);
    if (numCaps > 0)
    {
        if (HIDP_STATUS_SUCCESS != (HidP_GetValueCaps(HidP_Output,
                                                      OutputValueCaps.get(),
                                                      &numCaps,
                                                      *Ppd.get())))
        {
            return false;
        }
    }

    numValues = 0;
    for (int i = 0; i < Caps->NumberOutputValueCaps; i++)
    {
        if ((OutputValueCaps.get()[i]).IsRange)
        {
            numValues += (OutputValueCaps.get()[i]).Range.UsageMax
                - (OutputValueCaps.get()[i]).Range.UsageMin + 1;
        }
        else
        {
            numValues++;
        }
    }

    OutputDataLength = Caps->NumberOutputButtonCaps + numValues;

    OutputData = std::make_unique<HID_DATA[]>(OutputDataLength);

    dataIdx = 0;
    for (unsigned long i = 0; i < Caps->NumberOutputButtonCaps; i++)
    {
        unsigned long tmpSum{ 0 };

        if (i >= OutputDataLength) { return false; }

        // ULongAdd requires intsafe.h
        if (FAILED(ULongAdd(Caps->NumberOutputButtonCaps, (OutputValueCaps.get()[dataIdx]).Range.UsageMax, &tmpSum))) { return false; }

        if ((OutputValueCaps.get()[dataIdx]).Range.UsageMin == tmpSum) { return false; }

        (OutputData.get()[dataIdx]).IsButtonData = true;
        (OutputData.get()[dataIdx]).Status = HIDP_STATUS_SUCCESS;
        (OutputData.get()[dataIdx]).UsagePage = (OutputButtonCaps.get()[dataIdx]).UsagePage;

        if ((OutputButtonCaps.get()[dataIdx]).IsRange)
        {
            (OutputData.get()[dataIdx]).ButtonData.UsageMin = (OutputButtonCaps.get()[dataIdx]).Range.UsageMin;
            (OutputData.get()[dataIdx]).ButtonData.UsageMax = (OutputButtonCaps.get()[dataIdx]).Range.UsageMax;
        }
        else
        {
            (OutputData.get()[dataIdx]).ButtonData.UsageMin = (OutputData.get()[dataIdx]).ButtonData.UsageMax = (OutputButtonCaps.get()[dataIdx]).NotRange.Usage;
        }
        (OutputData.get()[dataIdx]).ButtonData.MaxUsageLength = HidP_MaxUsageListLength(HidP_Output,
                                                                                        (OutputButtonCaps.get()[dataIdx]).UsagePage,
                                                                                        *Ppd.get());
        (OutputData.get()[dataIdx]).ReportID = (OutputButtonCaps.get()[dataIdx]).ReportID;
    }

    for (int i = 0; i < Caps->NumberOutputValueCaps; i++)
    {
        if ((OutputValueCaps.get()[i]).IsRange)
        {
            for (USAGE usage = (OutputValueCaps.get()[i]).Range.UsageMin;
                 usage <= (OutputValueCaps.get()[i]).Range.UsageMax;
                 usage++)
            {
                HidDevice::SetHidData(OutputData, dataIdx, (OutputValueCaps.get()[i]).UsagePage,
                                      usage, (OutputValueCaps.get()[i]).ReportID);
            }
        }
        else
        {
            HidDevice::SetHidData(OutputData, dataIdx, (OutputValueCaps.get()[i]).UsagePage,
                                  (OutputValueCaps.get()[i]).NotRange.Usage, (OutputValueCaps.get()[i]).ReportID);
        }
    }

    // Setup feature buffers
    FeatureReportBuffer = std::make_unique<char[]>(Caps->FeatureReportByteLength);

    numCaps = Caps->NumberFeatureButtonCaps;
    if (numCaps > 0)
    {
        FeatureButtonCaps = std::make_unique<HIDP_BUTTON_CAPS[]>(numCaps);
        if (HIDP_STATUS_SUCCESS != (HidP_GetButtonCaps(HidP_Feature,
                                                       FeatureButtonCaps.get(),
                                                       &numCaps,
                                                       *Ppd.get())))
        {
            return false;
        }
    }

    numCaps = Caps->NumberFeatureValueCaps;
    if (numCaps > 0)
    {
        FeatureValueCaps = std::make_unique<HIDP_VALUE_CAPS[]>(numCaps);
        if (HIDP_STATUS_SUCCESS != (HidP_GetValueCaps(HidP_Feature,
                                                      FeatureValueCaps.get(),
                                                      &numCaps,
                                                      *Ppd.get())))
        {
            return false;
        }
    }

    numValues = 0;
    for (int i = 0; i < Caps->NumberFeatureValueCaps; i++)
    {
        if ((FeatureValueCaps.get()[i]).IsRange)
        {
            numValues += (FeatureValueCaps.get()[i]).Range.UsageMax
                - (FeatureValueCaps.get()[i]).Range.UsageMin + 1;
        }
        else
        {
            numValues++;
        }
    }

    if (FAILED(ULongAdd(Caps->NumberFeatureButtonCaps, numValues, &FeatureDataLength))) { return false; }

    FeatureData = std::make_unique<HID_DATA[]>(FeatureDataLength);

    dataIdx = 0;
    for (int i = 0; i < Caps->NumberFeatureButtonCaps; i++, dataIdx++)
    {
        (FeatureData.get()[dataIdx]).IsButtonData = true;
        (FeatureData.get()[dataIdx]).Status = HIDP_STATUS_SUCCESS;
        (FeatureData.get()[dataIdx]).UsagePage = (FeatureButtonCaps.get()[i]).UsagePage;

        if ((FeatureButtonCaps.get()[i]).IsRange)
        {
            (FeatureData.get()[dataIdx]).ButtonData.UsageMin = (FeatureButtonCaps.get()[i]).Range.UsageMin;
            (FeatureData.get()[dataIdx]).ButtonData.UsageMax = (FeatureButtonCaps.get()[i]).Range.UsageMax;
        }
        else
        {
            (FeatureData.get()[dataIdx]).ButtonData.UsageMin = (FeatureData.get()[dataIdx]).ButtonData.UsageMax = (FeatureButtonCaps.get()[i]).NotRange.Usage;
        }
        (FeatureData.get()[dataIdx]).ButtonData.MaxUsageLength = HidP_MaxUsageListLength(HidP_Feature,
                                                                                         (FeatureButtonCaps.get()[i]).UsagePage,
                                                                                         *Ppd.get());
        (FeatureData.get()[dataIdx]).ReportID = (FeatureButtonCaps.get()[i]).ReportID;
    }

    for (int i = 0; i < Caps->NumberFeatureValueCaps; i++)
    {
        if ((FeatureValueCaps.get()[i]).IsRange)
        {
            for (USAGE usage = (FeatureValueCaps.get()[i]).Range.UsageMin;
                 usage <= (FeatureValueCaps.get()[i]).Range.UsageMax;
                 usage++)
            {
                if (dataIdx >= FeatureDataLength) { return false; }
                HidDevice::SetHidData(FeatureData, dataIdx, (FeatureValueCaps.get()[i]).UsagePage,
                                      usage, (FeatureValueCaps.get()[i]).ReportID);
            }
        }
        else
        {
            if (dataIdx >= FeatureDataLength) { return false; }
            HidDevice::SetHidData(FeatureData, dataIdx, (FeatureValueCaps.get()[i]).UsagePage,
                                  (FeatureValueCaps.get()[i]).NotRange.Usage, (FeatureValueCaps.get()[i]).ReportID);
        }
    }

    return true;
}

HidDevices::HidDevices()
{
    devices.clear();
}

bool HidDevices::FindAllHidDevices()
{
    HDEVINFO hardwareDeviceInfo{};
    GUID hidGuid;

    // Delete all currently known devices
    devices.clear();

    HidD_GetHidGuid(&hidGuid);

    // Open a handle to the plug and play dev node
    hardwareDeviceInfo = SetupDiGetClassDevsA(&hidGuid,
                                              nullptr,
                                              nullptr,
                                              (DIGCF_PRESENT | DIGCF_DEVICEINTERFACE));

    if (hardwareDeviceInfo == INVALID_HANDLE_VALUE) { return false; }

    int i{ 0 };

    do
    {
        // Create unique_ptr for deviceInfo
        auto deviceInfoData = std::make_unique<SP_DEVICE_INTERFACE_DATA>();
        deviceInfoData->cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

        if (SetupDiEnumDeviceInterfaces(hardwareDeviceInfo,
                                        0,
                                        &hidGuid,
                                        i,
                                        deviceInfoData.get()))
        {
            unsigned long requiredLength{ 0 };

            // Determine required size required for device information
            SetupDiGetDeviceInterfaceDetailA(hardwareDeviceInfo,
                                             deviceInfoData.get(),
                                             nullptr,
                                             0,
                                             &requiredLength,
                                             nullptr);

            // Calculate size of structure + required device path length + null termination
            size_t structureSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_A) + requiredLength + 1;

            // Allocate memory for the structure
            auto ptr = std::make_unique<char[]>(structureSize);

            // Set cbsize member and ensure DevicePath is null terminated
            reinterpret_cast<SP_DEVICE_INTERFACE_DETAIL_DATA_A*>(ptr.get())->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_A);
            reinterpret_cast<SP_DEVICE_INTERFACE_DETAIL_DATA_A*>(ptr.get())->DevicePath[0] = '\0';  // Ensure null-terminated
            reinterpret_cast<SP_DEVICE_INTERFACE_DETAIL_DATA_A*>(ptr.get())->DevicePath[1] = '\0';  // Ensure null-terminated

            // Reassign ptr to properly typed ptr
            auto functionClassDeviceData = std::unique_ptr<SP_DEVICE_INTERFACE_DETAIL_DATA_A>(reinterpret_cast<SP_DEVICE_INTERFACE_DETAIL_DATA_A*>(ptr.release()));

            // Now get the device data for specified interface
            if (SetupDiGetDeviceInterfaceDetailA(hardwareDeviceInfo,
                                                 deviceInfoData.get(),
                                                 functionClassDeviceData.get(),
                                                 requiredLength,
                                                 nullptr,
                                                 nullptr))
            {
                // Create a new HidDevice pointer 
                HidDevicePtr hidDeviceInst = std::make_unique<HidDevice>(functionClassDeviceData->DevicePath);
                hidDeviceInst->Open();
                // Add new HidDevice to devices vector
                devices.push_back(std::move(hidDeviceInst));
            }
        }
        i++;
    } while (GetLastError() != ERROR_NO_MORE_ITEMS);

    return false;
}

std::ostream& operator<<(std::ostream& strm, const HidDevice& hd)
{

    strm << std::format("{:<50} VID: {:#06x} PID: {:#06x} UsagePage: {:#04x}, Usage: {:#04x}\n",
                        hd.ManufacturerString.empty() ? "Unknown USB HID Device" : hd.ManufacturerString + " " + hd.ProductString,
                        hd.Attributes->VendorID,
                        hd.Attributes->ProductID,
                        hd.Caps->UsagePage,
                        hd.Caps->Usage);
    return strm;
}

std::ostream& operator<<(std::ostream& strm, const HidDevices& hds)
{
    for (auto& hd : hds.devices)
    {
        strm << *(hd.get());
    }
    return strm;
}
