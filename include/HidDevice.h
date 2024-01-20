#pragma once

#include <memory>
#include <string>
#include <vector>
#include <Windows.h>
#include <hidsdi.h>
#include <SetupAPI.h>
#include <intsafe.h>

typedef struct _HID_DATA
{
    bool        IsButtonData;
    UCHAR       Reserved;
    USAGE       UsagePage;   // The usage page for which we are looking.
    ULONG       Status;      // The last status returned from the accessor function
    // when updating this field.
    ULONG       ReportID;    // ReportID for this given data structure
    bool        IsDataSet;   // Variable to track whether a given data structure
    //  has already been added to a report structure

    union
    {
        struct
        {
            ULONG                   UsageMin;       // Variables to track the usage minimum and max
            ULONG                   UsageMax;       // If equal, then only a single usage
            ULONG                   MaxUsageLength; // Usages buffer length.
            //std::vector<USAGE>      Usages;         // list of usages (buttons ``down'' on the device.

        } ButtonData;
        struct
        {
            USAGE       Usage;          // The usage describing this value
            USHORT      Reserved;

            ULONG       Value;
            LONG        ScaledValue;
        } ValueData;
    };
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

    bool UnpackReport();
    bool PackReport();

public:
    HidDevice(std::string DevicePath);
    ~HidDevice();
    bool IsOpen();
    void Close();
    bool Read();
    bool ReadOverlapped();
    bool Write();
    bool Open(bool HasReadAccess = false, bool HasWriteAccess = false, bool IsOverlapped = false, bool IsExclusive = false);
    bool FillDevice();
};

using HidDevicePtr = std::unique_ptr<HidDevice>;

class HidDevices
{
    std::vector<HidDevicePtr> devices;

public:
    HidDevices();
    bool FindAllHidDevices();
};
