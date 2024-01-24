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

bool HidDevice::UnpackReport(charBufferPtr& ReportBuffer,
                             unsigned short ReportBufferLength,
                             HIDP_REPORT_TYPE ReportType,
                             HidDataPtr& Data,
                             unsigned long DataLength,
                             std::unique_ptr<PHIDP_PREPARSED_DATA>& Ppd)
{
    unsigned char reportId = ReportBuffer.get()[0];

    for (unsigned long i = 0; i < DataLength; i++)
    {
        auto& dtaPtr = (Data.get()[i]);

        if (reportId == dtaPtr.ReportID)
        {
            if (dtaPtr.IsButtonData)
            {
                unsigned long numUsages = dtaPtr.ButtonData.MaxUsageLength;

                dtaPtr.Status = HidP_GetUsages(ReportType,
                                               dtaPtr.UsagePage,
                                               0,
                                               dtaPtr.ButtonData.Usages.data(),
                                               &numUsages,
                                               *(Ppd.get()),
                                               ReportBuffer.get(),
                                               ReportBufferLength);

                if (dtaPtr.Status != HIDP_STATUS_SUCCESS) { return false; }

                // Search through the usage list and remove those that
                //    correspond to usages outside the define ranged for this
                //    data structure.
                unsigned long nextUsage{ 0 };
                for (unsigned long j = 0; j < numUsages; j++)
                {
                    if (dtaPtr.ButtonData.UsageMin <= dtaPtr.ButtonData.Usages[j] &&
                        dtaPtr.ButtonData.Usages[j] <= dtaPtr.ButtonData.UsageMax)
                    {
                        dtaPtr.ButtonData.Usages[nextUsage++] = dtaPtr.ButtonData.Usages[j];
                    }
                }
                if (nextUsage < dtaPtr.ButtonData.MaxUsageLength)
                {
                    dtaPtr.ButtonData.Usages[nextUsage] = 0;
                }
            }
            else
            {
                dtaPtr.Status = HidP_GetUsageValue(ReportType,
                                                   dtaPtr.UsagePage,
                                                   0,
                                                   dtaPtr.ValueData.Usage,
                                                   &dtaPtr.ValueData.Value,
                                                   *(Ppd.get()),
                                                   ReportBuffer.get(),
                                                   ReportBufferLength);
                if (dtaPtr.Status != HIDP_STATUS_SUCCESS) { return false; }

                dtaPtr.Status = HidP_GetScaledUsageValue(ReportType,
                                                         dtaPtr.UsagePage,
                                                         0,
                                                         dtaPtr.ValueData.Usage,
                                                         &dtaPtr.ValueData.ScaledValue,
                                                         *(Ppd.get()),
                                                         ReportBuffer.get(),
                                                         ReportBufferLength);
                if (dtaPtr.Status != HIDP_STATUS_SUCCESS &&
                    dtaPtr.Status != HIDP_STATUS_NULL)
                {
                    return false;
                }
            }
            dtaPtr.IsDataSet = true;
        }
    }

    return true;
}

bool HidDevice::IsTarget(int vid, int pid, int usagepage, int usagecode)
{
    return Attributes->VendorID == vid &&
        Attributes->ProductID == pid &&
        Caps->UsagePage == usagepage &&
        Caps->Usage == usagecode;
}

USAGE HidDevice::getKeyPress()
{
    return (InputData.get())->ButtonData.Usages.front();
}

std::string HidDevice::LoadHidString(const std::function<BOOLEAN(HANDLE, PVOID, ULONG)> func) const
{
    auto wideString = std::make_unique<wchar_t[]>(256);
    if (func(device, wideString.get(), 256))
    {
        int sizeRequired = WideCharToMultiByte(CP_UTF8, 0, wideString.get(), -1, nullptr, 0, NULL, NULL);
        auto charString = std::make_unique<char[]>(sizeRequired);
        WideCharToMultiByte(CP_UTF8, 0, wideString.get(), -1, charString.get(), sizeRequired, NULL, NULL);
        return std::string(charString.get());
    }
    return {};
}

void HidDevice::SetHidData(HidDataPtr& ptr, const unsigned long offset, const USAGE up, const USAGE usage, const unsigned long rid)
{
    (ptr.get()[offset]).IsButtonData = false;
    (ptr.get()[offset]).Status = HIDP_STATUS_SUCCESS;
    (ptr.get()[offset]).UsagePage = up;
    (ptr.get()[offset]).ValueData.Usage = usage;
    (ptr.get()[offset]).ReportID = rid;
}

HidDevice::HidDevice(const std::string& DevicePath)
{
    this->DevicePath = DevicePath;
    this->device = INVALID_HANDLE_VALUE;
    this->Ppd = std::make_unique<PHIDP_PREPARSED_DATA>();
    this->Attributes = std::make_unique<HIDD_ATTRIBUTES>();
    this->Caps = std::make_unique<HIDP_CAPS>();
}

HidDevice::~HidDevice() { Close(); }

void HidDevice::Close()
{
    if (IsOpen())
    {
        CloseHandle(device);
        device = INVALID_HANDLE_VALUE;
    }
}

bool HidDevice::IsOpen() const
{
    return device != INVALID_HANDLE_VALUE;
}

bool HidDevice::Read()
{
    unsigned long bytesRead;

    if (!IsOpen() && OpenedForRead)
    {
        throw std::exception("HidDevice not open for read.");
    }

    if (!ReadFile(device,
                  InputReportBuffer.get(),
                  Caps->InputReportByteLength,
                  &bytesRead,
                  NULL))
    {
        return false;
    }
    if (bytesRead != Caps->InputReportByteLength) { return false; }

    return UnpackReport(InputReportBuffer,
                        Caps->InputReportByteLength,
                        HidP_Input,
                        InputData,
                        InputDataLength,
                        Ppd);
}

bool HidDevice::Open(bool HasReadAccess, bool HasWriteAccess, bool IsOverlapped, bool IsExclusive)
{
    // If HidDevice is already open (likely with other permissions), close it first.
    if (IsOpen()) { Close(); }

    unsigned long accessFlags{ 0 }, sharingFlags{ 0 };

    if (DevicePath.empty()) { return false; }

    if (HasReadAccess) { accessFlags |= GENERIC_READ; }
    if (HasWriteAccess) { accessFlags |= GENERIC_WRITE; }
    if (!IsExclusive) { sharingFlags = FILE_SHARE_READ | FILE_SHARE_WRITE; }

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
    return true;
}

bool HidDevice::FillDevice()
{
    unsigned short numCaps{ 0 };
    unsigned long numValues{ 0 };

    // Load Manufacturer and Product String
    ManufacturerString = LoadHidString(HidD_GetManufacturerString);
    ProductString = LoadHidString(HidD_GetProductString);

    // Allocate memory for input report
    InputReportBuffer = std::make_unique<char[]>(Caps->InputReportByteLength);

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
        const auto& ivcPtr = InputValueCaps[i];

        if (ivcPtr.IsRange)
        {
            numValues += ivcPtr.Range.UsageMax - ivcPtr.Range.UsageMin + 1;
            if (ivcPtr.Range.UsageMin > ivcPtr.Range.UsageMax) { return false; }
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
    if (InputDataLength > 0)
    {
        InputData = std::make_unique<HID_DATA[]>(InputDataLength);
    }

    // Fill in the button data

    unsigned long dataIdx{ 0 };
    for (int i = 0; i < Caps->NumberInputButtonCaps && InputData; i++, dataIdx++)
    {
        auto& idPtr = InputData[dataIdx];
        const auto& ibcPtr = InputButtonCaps[dataIdx];

        idPtr.IsButtonData = true;
        idPtr.Status = HIDP_STATUS_SUCCESS;
        idPtr.UsagePage = ibcPtr.UsagePage;
        if (ibcPtr.IsRange)
        {
            idPtr.ButtonData.UsageMin = ibcPtr.Range.UsageMin;
            idPtr.ButtonData.UsageMax = ibcPtr.Range.UsageMax;
        }
        else
        {
            idPtr.ButtonData.UsageMin = idPtr.ButtonData.UsageMax = ibcPtr.NotRange.Usage;
        }

        idPtr.ButtonData.MaxUsageLength = HidP_MaxUsageListLength(HidP_Input,
                                                                  ibcPtr.UsagePage,
                                                                  *Ppd.get());

        idPtr.ButtonData.Usages.resize(idPtr.ButtonData.MaxUsageLength);

        idPtr.ReportID = ibcPtr.ReportID;
    }

    // Fill in the value data

    for (int i = 0; i < Caps->NumberInputValueCaps && InputData; i++)
    {
        const auto& ivcPtr = InputValueCaps[i];

        if (ivcPtr.IsRange)
        {
            for (USAGE usage = ivcPtr.Range.UsageMin;
                 usage <= ivcPtr.Range.UsageMax;
                 usage++)
            {
                if (dataIdx >= (InputDataLength)) { return false; }
                HidDevice::SetHidData(InputData, dataIdx, ivcPtr.UsagePage,
                                      usage, ivcPtr.ReportID);
            }
        }
        else
        {
            HidDevice::SetHidData(InputData, dataIdx, ivcPtr.UsagePage,
                                  ivcPtr.NotRange.Usage, ivcPtr.ReportID);
        }
    }

    // Setup Output Data Buffers
    OutputReportBuffer = std::make_unique<char[]>(Caps->OutputReportByteLength);

    numCaps = Caps->NumberOutputButtonCaps;
    if (numCaps > 0)
    {
        OutputButtonCaps = std::make_unique<HIDP_BUTTON_CAPS[]>(numCaps);
        if (HIDP_STATUS_SUCCESS != (HidP_GetButtonCaps(HidP_Output,
                                                       OutputButtonCaps.get(),
                                                       &numCaps,
                                                       *Ppd.get())))
        {
            return false;
        }
    }

    numCaps = Caps->NumberOutputValueCaps;
    if (numCaps > 0)
    {
        OutputValueCaps = std::make_unique<HIDP_VALUE_CAPS[]>(numCaps);
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
        const auto& ovcPtr = OutputValueCaps[i];

        if (ovcPtr.IsRange)
        {
            numValues += ovcPtr.Range.UsageMax
                - ovcPtr.Range.UsageMin + 1;
        }
        else
        {
            numValues++;
        }
    }

    OutputDataLength = Caps->NumberOutputButtonCaps + numValues;

    if (OutputDataLength > 0)
    {
        OutputData = std::make_unique<HID_DATA[]>(OutputDataLength);
    }

    dataIdx = 0;
    for (unsigned long i = 0; i < Caps->NumberOutputButtonCaps && OutputData; i++, dataIdx++)
    {
        auto& odPtr = OutputData[dataIdx];
        const auto& obcPtr = OutputButtonCaps[dataIdx];

        if (i >= OutputDataLength) { return false; }

        odPtr.IsButtonData = true;
        odPtr.Status = HIDP_STATUS_SUCCESS;
        odPtr.UsagePage = obcPtr.UsagePage;

        if (obcPtr.IsRange)
        {
            odPtr.ButtonData.UsageMin = obcPtr.Range.UsageMin;
            odPtr.ButtonData.UsageMax = obcPtr.Range.UsageMax;
        }
        else
        {
            odPtr.ButtonData.UsageMin = odPtr.ButtonData.UsageMax = obcPtr.NotRange.Usage;
        }
        odPtr.ButtonData.MaxUsageLength = HidP_MaxUsageListLength(HidP_Output,
                                                                  obcPtr.UsagePage,
                                                                  *Ppd.get());
        odPtr.ButtonData.Usages.resize(odPtr.ButtonData.MaxUsageLength);

        odPtr.ReportID = obcPtr.ReportID;
    }

    for (int i = 0; i < Caps->NumberOutputValueCaps && OutputData; i++)
    {
        const auto& ovcPtr = (OutputValueCaps.get()[i]);
        if (ovcPtr.IsRange)
        {
            for (USAGE usage = ovcPtr.Range.UsageMin;
                 usage <= ovcPtr.Range.UsageMax;
                 usage++)
            {
                HidDevice::SetHidData(OutputData, dataIdx, ovcPtr.UsagePage,
                                      usage, ovcPtr.ReportID);
            }
        }
        else
        {
            HidDevice::SetHidData(OutputData, dataIdx, ovcPtr.UsagePage,
                                  ovcPtr.NotRange.Usage, ovcPtr.ReportID);
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

    FeatureDataLength = Caps->NumberFeatureButtonCaps + numValues;

    if (FeatureDataLength > 0)
    {
        FeatureData = std::make_unique<HID_DATA[]>(FeatureDataLength);
    }

    dataIdx = 0;
    for (int i = 0; i < Caps->NumberFeatureButtonCaps && FeatureData; i++, dataIdx++)
    {
        auto& fdPtr = FeatureData[dataIdx];
        const auto& fbcPtr = FeatureButtonCaps[i];

        fdPtr.IsButtonData = true;
        fdPtr.Status = HIDP_STATUS_SUCCESS;
        fdPtr.UsagePage = fbcPtr.UsagePage;

        if (fbcPtr.IsRange)
        {
            fdPtr.ButtonData.UsageMin = fbcPtr.Range.UsageMin;
            fdPtr.ButtonData.UsageMax = fbcPtr.Range.UsageMax;
        }
        else
        {
            fdPtr.ButtonData.UsageMin = fdPtr.ButtonData.UsageMax = fbcPtr.NotRange.Usage;
        }
        fdPtr.ButtonData.MaxUsageLength = HidP_MaxUsageListLength(HidP_Feature,
                                                                  fbcPtr.UsagePage,
                                                                  *Ppd.get());

        fdPtr.ButtonData.Usages.resize(fdPtr.ButtonData.MaxUsageLength);
        fdPtr.ReportID = fbcPtr.ReportID;
    }

    for (int i = 0; i < Caps->NumberFeatureValueCaps && FeatureData; i++)
    {
        const auto& fvcPtr = (FeatureValueCaps.get()[i]);

        if (fvcPtr.IsRange)
        {
            for (USAGE usage = fvcPtr.Range.UsageMin;
                 usage <= fvcPtr.Range.UsageMax;
                 usage++)
            {
                if (dataIdx >= FeatureDataLength) { return false; }
                HidDevice::SetHidData(FeatureData, dataIdx, fvcPtr.UsagePage,
                                      usage, fvcPtr.ReportID);
            }
        }
        else
        {
            if (dataIdx >= FeatureDataLength) { return false; }
            HidDevice::SetHidData(FeatureData, dataIdx, fvcPtr.UsagePage,
                                  fvcPtr.NotRange.Usage, fvcPtr.ReportID);
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

std::vector<HidDevicePtr>& HidDevices::getDevices()
{
    return devices;
}

const std::vector<HidDevicePtr>& HidDevices::getDevices() const
{
    return devices;
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
