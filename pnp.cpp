/*++

Copyright (c) 1996    Microsoft Corporation

Module Name:

    pnp.c

Abstract:

    This module contains the code
    for finding, adding, removing, and identifying hid devices.

Environment:

    User mode

**** Originally found in hclient sample application provided from Microsoft.

--*/

#include <basetyps.h>
#include <stdlib.h>
#include <wtypes.h>
#include <setupapi.h>
#include "hidsdi.h"
#include "hid.h"
#include <strsafe.h>
#include <intsafe.h>

#pragma warning(disable:28146) // Warning is meant for kernel mode drivers 

bool FindKnownHidDevices(
    OUT PHID_DEVICE*    HidDevices,     // A array of struct _HID_DEVICE
    OUT PULONG          NumberDevices   // the length of this array.
)
/*++
Routine Description:
   Do the required PnP things in order to find all the HID devices in
   the system at this time.
--*/
{
    HDEVINFO                            hardwareDeviceInfo = INVALID_HANDLE_VALUE;
    SP_DEVICE_INTERFACE_DATA            deviceInfoData{};
    ULONG                               i = 0;
    bool                                done = false;
    PHID_DEVICE                         hidDeviceInst = nullptr;
    GUID                                hidGuid;
    PSP_DEVICE_INTERFACE_DETAIL_DATA_A  functionClassDeviceData = nullptr;
    ULONG                               requiredLength = 0;
    PHID_DEVICE                         newHidDevices = nullptr;


    HidD_GetHidGuid(&hidGuid);

    *HidDevices = nullptr;
    *NumberDevices = 0;

    //
    // Open a handle to the plug and play dev node.
    //
    hardwareDeviceInfo = SetupDiGetClassDevsA(&hidGuid,
                                              nullptr, // Define no enumerator (global)
                                              nullptr, // Define no
                                              (DIGCF_PRESENT | // Only Devices present
                                               DIGCF_DEVICEINTERFACE)); // Function class devices.

    if (hardwareDeviceInfo == INVALID_HANDLE_VALUE)
    {
        return false;
    }

    //
    // Take a wild guess to start
    //

    *NumberDevices = 4;

    deviceInfoData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

    while (!done)
    {
        *NumberDevices *= 2;

        if (*HidDevices)
        {
            newHidDevices = static_cast<PHID_DEVICE>(realloc(*HidDevices, (*NumberDevices * sizeof(HID_DEVICE))));

            if (newHidDevices == nullptr)
            {
                free(*HidDevices);
                return false;
            }

            *HidDevices = newHidDevices;
        }
        else
        {
            *HidDevices = static_cast<PHID_DEVICE>(calloc(*NumberDevices, sizeof(HID_DEVICE)));
        }

        if (*HidDevices == nullptr)
        {
            if (newHidDevices != nullptr)
            {
                free(newHidDevices);
                newHidDevices = nullptr;
                hidDeviceInst = nullptr;
            }
            return false;
        }

        hidDeviceInst = *HidDevices + i;

        for (; i < *NumberDevices; i++, hidDeviceInst++)
        {
            //
            // Initialize an empty HID_DEVICE
            //
            RtlZeroMemory(hidDeviceInst, sizeof(HID_DEVICE));

            hidDeviceInst->HidDevice = INVALID_HANDLE_VALUE;

            if (SetupDiEnumDeviceInterfaces(hardwareDeviceInfo,
                                            0, // No care about specific PDOs
                                            &hidGuid,
                                            i,
                                            &deviceInfoData))
            {
                //
                // allocate a function class device data structure to receive the
                // goods about this particular device.
                //

                SetupDiGetDeviceInterfaceDetailA(
                    hardwareDeviceInfo,
                    &deviceInfoData,
                    nullptr, // probing so no output buffer yet
                    0, // probing so output buffer length of zero
                    &requiredLength,
                    nullptr); // not interested in the specific dev-node


                functionClassDeviceData = static_cast<PSP_DEVICE_INTERFACE_DETAIL_DATA_A>(malloc(requiredLength));
                if (functionClassDeviceData)
                {
                    functionClassDeviceData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_A);
                    ZeroMemory(functionClassDeviceData->DevicePath, sizeof(functionClassDeviceData->DevicePath));
                }
                else
                {
                    if (*HidDevices != nullptr) 
                    {
                        free(*HidDevices);
                        *HidDevices = nullptr;
                    }
                    return false;
                }

                //
                // Retrieve the information from Plug and Play.
                //

                if (SetupDiGetDeviceInterfaceDetailA(
                    hardwareDeviceInfo,
                    &deviceInfoData,
                    functionClassDeviceData,
                    requiredLength,
                    nullptr,
                    nullptr))
                {
                    //
                    // Open device with just generic query abilities to begin with
                    //

                    if (!OpenHidDevice(functionClassDeviceData->DevicePath,
                                       false,      // ReadAccess - none
                                       false,      // WriteAccess - none
                                       false,       // Overlapped - no
                                       false,       // Exclusive - no
                                       hidDeviceInst))
                    {
                        //
                        // Save the device path so it can be still listed.
                        //
                        int     iDevicePathSize;

                        iDevicePathSize = static_cast<int>(strnlen(functionClassDeviceData->DevicePath, MAX_PATH) + 1);

                        hidDeviceInst->DevicePath = new char[iDevicePathSize];

                        if (hidDeviceInst->DevicePath != nullptr)
                        {
                            StringCbCopyA(hidDeviceInst->DevicePath, iDevicePathSize, functionClassDeviceData->DevicePath);
                        }
                        else
                        {
                            return false;
                        }
                    }
                }

                free(functionClassDeviceData);
                functionClassDeviceData = nullptr;
            }
            else
            {
                if (GetLastError() == ERROR_NO_MORE_ITEMS)
                {
                    done = true;
                    break;
                }
            }
        }
    }

    *NumberDevices = i;

    if (hardwareDeviceInfo != INVALID_HANDLE_VALUE)
    {
        SetupDiDestroyDeviceInfoList(hardwareDeviceInfo);
        hardwareDeviceInfo = INVALID_HANDLE_VALUE;
    }

    return done;
}

bool OpenHidDevice(
    _In_     LPSTR          DevicePath,
    _In_     bool           HasReadAccess,
    _In_     bool           HasWriteAccess,
    _In_     bool           IsOverlapped,
    _In_     bool           IsExclusive,
    _Out_    PHID_DEVICE    HidDevice
)
/*++
RoutineDescription:
    Given the HardwareDeviceInfo, representing a handle to the plug and
    play information, and deviceInfoData, representing a specific hid device,
    open that device and fill in all the relivant information in the given
    HID_DEVICE structure.

    return if the open and initialization was successfull or not.

--*/
{
    DWORD   accessFlags = 0;
    DWORD   sharingFlags = 0;
    int     iDevicePathSize;

    RtlZeroMemory(HidDevice, sizeof(HID_DEVICE));
    HidDevice->HidDevice = INVALID_HANDLE_VALUE;

    if (DevicePath == nullptr)
    {
        return false;
    }

    iDevicePathSize = static_cast<int>(strnlen(DevicePath, MAX_PATH) + 1);

    HidDevice->DevicePath = static_cast<PCHAR>(malloc(iDevicePathSize));

    if (HidDevice->DevicePath == nullptr)
    {
        return false;
    }

    StringCbCopyA(HidDevice->DevicePath, iDevicePathSize, DevicePath);

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

    //
    //  The hid.dll api's do not pass the overlapped structure into deviceiocontrol
    //  so to use them we must have a non overlapped device.  If the request is for
    //  an overlapped device we will close the device below and get a handle to an
    //  overlapped device
    //

    HidDevice->HidDevice = CreateFileA(DevicePath,
                                       accessFlags,
                                       sharingFlags,
                                       nullptr,        // no SECURITY_ATTRIBUTES structure
                                       OPEN_EXISTING, // No special create flags
                                       0,   // Open device as non-overlapped so we can get data
                                       nullptr);       // No template file

    if (HidDevice->HidDevice == INVALID_HANDLE_VALUE)
    {
        CloseHidDevice(HidDevice);
        return false;
    }

    HidDevice->OpenedForRead = HasReadAccess;
    HidDevice->OpenedForWrite = HasWriteAccess;
    HidDevice->OpenedOverlapped = IsOverlapped;
    HidDevice->OpenedExclusive = IsExclusive;

    //
    // If the device was not opened as overlapped, then fill in the rest of the
    //  HidDevice structure.  However, if opened as overlapped, this handle cannot
    //  be used in the calls to the HidD_ exported functions since each of these
    //  functions does synchronous I/O.
    //

    if (!HidD_GetPreparsedData(HidDevice->HidDevice, &HidDevice->Ppd) ||
        !HidD_GetAttributes(HidDevice->HidDevice, &HidDevice->Attributes) ||
        !HidP_GetCaps(HidDevice->Ppd, &HidDevice->Caps) || 
        !FillDeviceInfo(HidDevice))
    {
        CloseHidDevice(HidDevice);
        return false;
    }

    if (IsOverlapped)
    {
        CloseHandle(HidDevice->HidDevice);
        HidDevice->HidDevice = INVALID_HANDLE_VALUE;

        HidDevice->HidDevice = CreateFileA(DevicePath,
                                           accessFlags,
                                           sharingFlags,
                                           nullptr,        // no SECURITY_ATTRIBUTES structure
                                           OPEN_EXISTING, // No special create flags
                                           FILE_FLAG_OVERLAPPED, // Now we open the device as overlapped
                                           nullptr);       // No template file

        if (HidDevice->HidDevice == INVALID_HANDLE_VALUE)
        {
            CloseHidDevice(HidDevice);
            return false;
        }
    }

    return true;
}

bool FillDeviceInfo(
    IN  PHID_DEVICE HidDevice
)
{
    ULONG               numValues;
    USHORT              numCaps;
    PHIDP_BUTTON_CAPS   buttonCaps;
    PHIDP_VALUE_CAPS    valueCaps;
    PHID_DATA           data;
    ULONG               i;
    USAGE               usage;
    UINT                dataIdx;
    ULONG               newFeatureDataLength;
    ULONG               tmpSum;

    //
    // setup Input Data buffers.
    //

    //
    // Allocate memory to hold on input report
    //

    HidDevice->InputReportBuffer = static_cast<PCHAR>(calloc(HidDevice->Caps.InputReportByteLength, sizeof(CHAR)));


    //
    // Allocate memory to hold the button and value capabilities.
    // NumberXXCaps is in terms of array elements.
    //

    HidDevice->InputButtonCaps = buttonCaps = static_cast<PHIDP_BUTTON_CAPS>(calloc(HidDevice->Caps.NumberInputButtonCaps, sizeof(HIDP_BUTTON_CAPS)));

    if (buttonCaps == nullptr)
    {
        return false;
    }

    HidDevice->InputValueCaps = valueCaps = static_cast<PHIDP_VALUE_CAPS>(calloc(HidDevice->Caps.NumberInputValueCaps, sizeof(HIDP_VALUE_CAPS)));

    if (valueCaps == nullptr)
    {
        return false;
    }

    //
    // Have the HidP_X functions fill in the capability structure arrays.
    //

    numCaps = HidDevice->Caps.NumberInputButtonCaps;

    if (numCaps > 0)
    {
        if (HIDP_STATUS_SUCCESS != (HidP_GetButtonCaps(HidP_Input,
                                                       buttonCaps,
                                                       &numCaps,
                                                       HidDevice->Ppd)))
        {
            return false;
        }
    }

    numCaps = HidDevice->Caps.NumberInputValueCaps;

    if (numCaps > 0)
    {
        if (HIDP_STATUS_SUCCESS != (HidP_GetValueCaps(HidP_Input,
                                                      valueCaps,
                                                      &numCaps,
                                                      HidDevice->Ppd)))
        {
            return false;
        }
    }


    //
    // Depending on the device, some value caps structures may represent more
    // than one value.  (A range).  In the interest of being verbose, over
    // efficient, we will expand these so that we have one and only one
    // struct _HID_DATA for each value.
    //
    // To do this we need to count up the total number of values are listed
    // in the value caps structure.  For each element in the array we test
    // for range if it is a range then UsageMax and UsageMin describe the
    // usages for this range INCLUSIVE.
    //

    numValues = 0;
    for (i = 0; i < HidDevice->Caps.NumberInputValueCaps; i++, valueCaps++)
    {
        if (valueCaps->IsRange)
        {
            numValues += valueCaps->Range.UsageMax - valueCaps->Range.UsageMin + 1;
            if (valueCaps->Range.UsageMin > valueCaps->Range.UsageMax)
            {
                return false;
            }
        }
        else
        {
            numValues++;
        }
    }

    valueCaps = HidDevice->InputValueCaps;

    //
    // Allocate a buffer to hold the struct _HID_DATA structures.
    // One element for each set of buttons, and one element for each value
    // found.
    //

    HidDevice->InputDataLength = HidDevice->Caps.NumberInputButtonCaps
        + numValues;

    HidDevice->InputData = data = static_cast<PHID_DATA>(calloc(HidDevice->InputDataLength, sizeof(HID_DATA)));

    if (data == nullptr)
    {
        return false;
    }

    //
    // Fill in the button data
    //
    dataIdx = 0;
    for (i = 0;
         i < HidDevice->Caps.NumberInputButtonCaps;
         i++, data++, buttonCaps++, dataIdx++)
    {
        data->IsButtonData = true;
        data->Status = HIDP_STATUS_SUCCESS;
        data->UsagePage = buttonCaps->UsagePage;
        if (buttonCaps->IsRange)
        {
            data->ButtonData.UsageMin = buttonCaps->Range.UsageMin;
            data->ButtonData.UsageMax = buttonCaps->Range.UsageMax;
        }
        else
        {
            data->ButtonData.UsageMin = data->ButtonData.UsageMax = buttonCaps->NotRange.Usage;
        }

        data->ButtonData.MaxUsageLength = HidP_MaxUsageListLength(
            HidP_Input,
            buttonCaps->UsagePage,
            HidDevice->Ppd);
        data->ButtonData.Usages = static_cast<PUSAGE>(calloc(data->ButtonData.MaxUsageLength, sizeof(USAGE)));

        data->ReportID = buttonCaps->ReportID;
    }

    //
    // Fill in the value data
    //

    for (i = 0; i < HidDevice->Caps.NumberInputValueCaps; i++, valueCaps++)
    {
        if (valueCaps->IsRange)
        {
            for (usage = valueCaps->Range.UsageMin;
                 usage <= valueCaps->Range.UsageMax;
                 usage++)
            {
                if (dataIdx >= (HidDevice->InputDataLength))
                {
                    return false;
                }
                data->IsButtonData = false;
                data->Status = HIDP_STATUS_SUCCESS;
                data->UsagePage = valueCaps->UsagePage;
                data->ValueData.Usage = usage;
                data->ReportID = valueCaps->ReportID;
                data++;
                dataIdx++;
            }
        }
        else
        {
            if (dataIdx >= (HidDevice->InputDataLength))
            {
                return false;
            }
            data->IsButtonData = false;
            data->Status = HIDP_STATUS_SUCCESS;
            data->UsagePage = valueCaps->UsagePage;
            data->ValueData.Usage = valueCaps->NotRange.Usage;
            data->ReportID = valueCaps->ReportID;
            data++;
            dataIdx++;
        }
    }

    //
    // setup Output Data buffers.
    //

    HidDevice->OutputReportBuffer = static_cast<PCHAR>(calloc(HidDevice->Caps.OutputReportByteLength, sizeof(CHAR)));

    HidDevice->OutputButtonCaps = buttonCaps = static_cast<PHIDP_BUTTON_CAPS>(calloc(HidDevice->Caps.NumberOutputButtonCaps, sizeof(HIDP_BUTTON_CAPS)));

    if (buttonCaps == nullptr)
    {
        return false;
    }

    HidDevice->OutputValueCaps = valueCaps = static_cast<PHIDP_VALUE_CAPS>(calloc(HidDevice->Caps.NumberOutputValueCaps, sizeof(HIDP_VALUE_CAPS)));

    if (valueCaps == nullptr)
    {
        return false;
    }

    numCaps = HidDevice->Caps.NumberOutputButtonCaps;
    if (numCaps > 0)
    {
        if (HIDP_STATUS_SUCCESS != (HidP_GetButtonCaps(HidP_Output,
                                                       buttonCaps,
                                                       &numCaps,
                                                       HidDevice->Ppd)))
        {
            return false;
        }
    }

    numCaps = HidDevice->Caps.NumberOutputValueCaps;
    if (numCaps > 0)
    {
        if (HIDP_STATUS_SUCCESS != (HidP_GetValueCaps(HidP_Output,
                                                      valueCaps,
                                                      &numCaps,
                                                      HidDevice->Ppd)))
        {
            return false;
        }
    }

    numValues = 0;
    for (i = 0; i < HidDevice->Caps.NumberOutputValueCaps; i++, valueCaps++)
    {
        if (valueCaps->IsRange)
        {
            numValues += valueCaps->Range.UsageMax
                - valueCaps->Range.UsageMin + 1;
        }
        else
        {
            numValues++;
        }
    }
    valueCaps = HidDevice->OutputValueCaps;

    HidDevice->OutputDataLength = HidDevice->Caps.NumberOutputButtonCaps
        + numValues;

    HidDevice->OutputData = data = static_cast<PHID_DATA>(calloc(HidDevice->OutputDataLength, sizeof(HID_DATA)));

    if (data == nullptr)
    {
        return false;
    }

    for (i = 0;
         i < HidDevice->Caps.NumberOutputButtonCaps;
         i++, data++, buttonCaps++)
    {
        if (i >= HidDevice->OutputDataLength)
        {
            return false;
        }

        if (FAILED(ULongAdd((HidDevice->Caps).NumberOutputButtonCaps,
                            (valueCaps->Range).UsageMax, &tmpSum)))
        {
            return false;
        }

        if ((valueCaps->Range).UsageMin == tmpSum)
        {
            return false;
        }

        data->IsButtonData = true;
        data->Status = HIDP_STATUS_SUCCESS;
        data->UsagePage = buttonCaps->UsagePage;

        if (buttonCaps->IsRange)
        {
            data->ButtonData.UsageMin = buttonCaps->Range.UsageMin;
            data->ButtonData.UsageMax = buttonCaps->Range.UsageMax;
        }
        else
        {
            data->ButtonData.UsageMin = data->ButtonData.UsageMax = buttonCaps->NotRange.Usage;
        }

        data->ButtonData.MaxUsageLength = HidP_MaxUsageListLength(
            HidP_Output,
            buttonCaps->UsagePage,
            HidDevice->Ppd);

        data->ButtonData.Usages = static_cast<PUSAGE>(calloc(data->ButtonData.MaxUsageLength, sizeof(USAGE)));

        data->ReportID = buttonCaps->ReportID;
    }

    for (i = 0; i < HidDevice->Caps.NumberOutputValueCaps; i++, valueCaps++)
    {
        if (valueCaps->IsRange)
        {
            for (usage = valueCaps->Range.UsageMin;
                 usage <= valueCaps->Range.UsageMax;
                 usage++)
            {
                data->IsButtonData = false;
                data->Status = HIDP_STATUS_SUCCESS;
                data->UsagePage = valueCaps->UsagePage;
                data->ValueData.Usage = usage;
                data->ReportID = valueCaps->ReportID;
                data++;
            }
        }
        else
        {
            data->IsButtonData = false;
            data->Status = HIDP_STATUS_SUCCESS;
            data->UsagePage = valueCaps->UsagePage;
            data->ValueData.Usage = valueCaps->NotRange.Usage;
            data->ReportID = valueCaps->ReportID;
            data++;
        }
    }

    //
    // setup Feature Data buffers.
    //

    HidDevice->FeatureReportBuffer = static_cast<PCHAR>(calloc(HidDevice->Caps.FeatureReportByteLength, sizeof(CHAR)));

    HidDevice->FeatureButtonCaps = buttonCaps = static_cast<PHIDP_BUTTON_CAPS>(calloc(HidDevice->Caps.NumberFeatureButtonCaps, sizeof(HIDP_BUTTON_CAPS)));

    if (buttonCaps == nullptr)
    {
        return false;
    }

    HidDevice->FeatureValueCaps = valueCaps = (PHIDP_VALUE_CAPS)
        calloc(HidDevice->Caps.NumberFeatureValueCaps, sizeof(HIDP_VALUE_CAPS));

    if (valueCaps == nullptr)
    {
        return false;
    }

    numCaps = HidDevice->Caps.NumberFeatureButtonCaps;
    if (numCaps > 0)
    {
        if (HIDP_STATUS_SUCCESS != (HidP_GetButtonCaps(HidP_Feature,
                                                       buttonCaps,
                                                       &numCaps,
                                                       HidDevice->Ppd)))
        {
            return false;
        }
    }

    numCaps = HidDevice->Caps.NumberFeatureValueCaps;
    if (numCaps > 0)
    {
        if (HIDP_STATUS_SUCCESS != (HidP_GetValueCaps(HidP_Feature,
                                                      valueCaps,
                                                      &numCaps,
                                                      HidDevice->Ppd)))
        {
            return false;
        }
    }

    numValues = 0;
    for (i = 0; i < HidDevice->Caps.NumberFeatureValueCaps; i++, valueCaps++)
    {
        if (valueCaps->IsRange)
        {
            numValues += valueCaps->Range.UsageMax
                - valueCaps->Range.UsageMin + 1;
        }
        else
        {
            numValues++;
        }
    }
    valueCaps = HidDevice->FeatureValueCaps;

    if (FAILED(ULongAdd(HidDevice->Caps.NumberFeatureButtonCaps,
                        numValues, &newFeatureDataLength)))
    {
        return false;
    }

    HidDevice->FeatureDataLength = newFeatureDataLength;

    HidDevice->FeatureData = data = static_cast<PHID_DATA>(calloc(HidDevice->FeatureDataLength, sizeof(HID_DATA)));

    if (data == nullptr)
    {
        return false;
    }

    dataIdx = 0;
    for (i = 0;
         i < HidDevice->Caps.NumberFeatureButtonCaps;
         i++, data++, buttonCaps++, dataIdx++)
    {
        data->IsButtonData = true;
        data->Status = HIDP_STATUS_SUCCESS;
        data->UsagePage = buttonCaps->UsagePage;

        if (buttonCaps->IsRange)
        {
            data->ButtonData.UsageMin = buttonCaps->Range.UsageMin;
            data->ButtonData.UsageMax = buttonCaps->Range.UsageMax;
        }
        else
        {
            data->ButtonData.UsageMin = data->ButtonData.UsageMax = buttonCaps->NotRange.Usage;
        }

        data->ButtonData.MaxUsageLength = HidP_MaxUsageListLength(
            HidP_Feature,
            buttonCaps->UsagePage,
            HidDevice->Ppd);
        data->ButtonData.Usages = static_cast<PUSAGE>(calloc(data->ButtonData.MaxUsageLength, sizeof(USAGE)));

        data->ReportID = buttonCaps->ReportID;
    }

    for (i = 0; i < HidDevice->Caps.NumberFeatureValueCaps; i++, valueCaps++)
    {
        if (valueCaps->IsRange)
        {
            for (usage = valueCaps->Range.UsageMin;
                 usage <= valueCaps->Range.UsageMax;
                 usage++)
            {
                if (dataIdx >= (HidDevice->FeatureDataLength))
                {
                    return false;
                }
                data->IsButtonData = false;
                data->Status = HIDP_STATUS_SUCCESS;
                data->UsagePage = valueCaps->UsagePage;
                data->ValueData.Usage = usage;
                data->ReportID = valueCaps->ReportID;
                data++;
                dataIdx++;
            }
        }
        else
        {
            if (dataIdx >= (HidDevice->FeatureDataLength))
            {
                return false;
            }
            data->IsButtonData = false;
            data->Status = HIDP_STATUS_SUCCESS;
            data->UsagePage = valueCaps->UsagePage;
            data->ValueData.Usage = valueCaps->NotRange.Usage;
            data->ReportID = valueCaps->ReportID;
            data++;
            dataIdx++;
        }
    }
    
    return true;
}

void CloseHidDevices(
    IN  PHID_DEVICE HidDevices,
    IN  ULONG       NumberDevices
)
{
    for (unsigned int Index = 0; Index < NumberDevices; Index++)
    {
        CloseHidDevice(HidDevices + Index);
    }

    return;
}

void CloseHidDevice(
    IN PHID_DEVICE HidDevice
)
{
    if (HidDevice->DevicePath != nullptr)
    {
        free(HidDevice->DevicePath);
        HidDevice->DevicePath = nullptr;
    }

    if (INVALID_HANDLE_VALUE != HidDevice->HidDevice)
    {
        CloseHandle(HidDevice->HidDevice);
        HidDevice->HidDevice = INVALID_HANDLE_VALUE;
    }

    if (HidDevice->Ppd != nullptr)
    {
        HidD_FreePreparsedData(HidDevice->Ppd);
        HidDevice->Ppd = nullptr;
    }

    if (HidDevice->InputReportBuffer != nullptr)
    {
        free(HidDevice->InputReportBuffer);
        HidDevice->InputReportBuffer = nullptr;
    }

    if (HidDevice->InputData != nullptr)
    {
        free(HidDevice->InputData);
        HidDevice->InputData = nullptr;
    }

    if (HidDevice->InputButtonCaps != nullptr)
    {
        free(HidDevice->InputButtonCaps);
        HidDevice->InputButtonCaps = nullptr;
    }

    if (HidDevice->InputValueCaps != nullptr)
    {
        free(HidDevice->InputValueCaps);
        HidDevice->InputValueCaps = nullptr;
    }

    if (HidDevice->OutputReportBuffer != nullptr)
    {
        free(HidDevice->OutputReportBuffer);
        HidDevice->OutputReportBuffer = nullptr;
    }

    if (HidDevice->OutputData != nullptr)
    {
        free(HidDevice->OutputData);
        HidDevice->OutputData = nullptr;
    }

    if (HidDevice->OutputButtonCaps != nullptr)
    {
        free(HidDevice->OutputButtonCaps);
        HidDevice->OutputButtonCaps = nullptr;
    }

    if (HidDevice->OutputValueCaps != nullptr)
    {
        free(HidDevice->OutputValueCaps);
        HidDevice->OutputValueCaps = nullptr;
    }

    if (HidDevice->FeatureReportBuffer != nullptr)
    {
        free(HidDevice->FeatureReportBuffer);
        HidDevice->FeatureReportBuffer = nullptr;
    }

    if (HidDevice->FeatureData != nullptr)
    {
        free(HidDevice->FeatureData);
        HidDevice->FeatureData = nullptr;
    }

    if (HidDevice->FeatureButtonCaps != nullptr)
    {
        free(HidDevice->FeatureButtonCaps);
        HidDevice->FeatureButtonCaps = nullptr;
    }

    if (HidDevice->FeatureValueCaps != nullptr)
    {
        free(HidDevice->FeatureValueCaps);
        HidDevice->FeatureValueCaps = nullptr;
    }

    return;
}

