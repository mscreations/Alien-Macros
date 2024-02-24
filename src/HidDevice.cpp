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
#include <stdexcept>
#include <format>
#include <thread>
#include <atomic>
#include <csignal>

 /// <summary>
 /// Unpacks HID report into the specified buffer
 /// </summary>
 /// <param name="ReportBuffer">Buffer to read from</param>
 /// <param name="ReportBufferLength">Length of read buffer</param>
 /// <param name="ReportType">Report type</param>
 /// <param name="Data">Output buffer</param>
 /// <param name="DataLength">Output buffer length</param>
 /// <param name="Ppd">Preparsed data</param>
 /// <returns></returns>
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

/// <summary>
/// Returns an object representing the target info for this device. This gets matched against
/// the known devices.
/// </summary>
/// <returns>TargetDevice object with parameters from this HidDevice</returns>
TargetDevice HidDevice::getTargetInfo() const
{
    return TargetDevice(Attributes->VendorID, Attributes->ProductID, Caps->UsagePage, Caps->Usage);
}

/// <summary>
/// Checks if HidDevice is the target device. Likely going to be deprecated.
/// </summary>
/// <param name="vid">Vendor ID</param>
/// <param name="pid">Product ID</param>
/// <param name="usagepage">Usage Page</param>
/// <param name="usagecode">Usage Code</param>
/// <returns>True if current device matches entered device details / false otherwise</returns>
bool HidDevice::IsTarget(int vid, int pid, int usagepage, int usagecode)
{
    return Attributes->VendorID == vid &&
        Attributes->ProductID == pid &&
        Caps->UsagePage == usagepage &&
        Caps->Usage == usagecode;
}

/// <summary>
/// Retrieves the first usage code received after a read operation
/// </summary>
/// <remarks>This probably should return the entire vector, but so far I have only 
/// required the first element for this project to work</remarks>
/// <returns>USAGE key scan code.</returns>
USAGE HidDevice::getKeyPress() const
{
    return (InputData.get())->ButtonData.Usages.front();
}

std::string HidDevice::getDevicePath() const
{
    return DevicePath;
}

/// <summary>
/// Load's a string using the specified function. This is intended for the HidD_GetManufacturerString
/// and HidD_GetProductString. Automatically loads the data into a wide string and then converts it
/// into a std::string.
/// </summary>
/// <param name="func">Function to call. Should be HidD_GetManufacturerString or
/// HidD_GetProductString</param>
/// <returns>std::string received from function call.</returns>
std::string HidDevice::LoadHidString(const std::function<BOOLEAN(HANDLE, PVOID, ULONG)> func) const
{
    std::wstring wideString(256, 0);
    if (func(device, wideString.data(), 256))
    {
        int sizeRequired = WideCharToMultiByte(CP_UTF8, 0, wideString.data(), -1, nullptr, 0, NULL, NULL);
        std::string charString(sizeRequired, 0);
        WideCharToMultiByte(CP_UTF8, 0, wideString.data(), -1, charString.data(), sizeRequired, NULL, NULL);
        return charString;
    }
    return {};
}

/// <summary>
/// Convenience function to set multiple data parameters
/// </summary>
/// <param name="ptr">Pointer to HidDevice array</param>
/// <param name="offset">Offset into array</param>
/// <param name="up">Usage Page</param>
/// <param name="usage">Usage Code</param>
/// <param name="rid">ReportID</param>
void HidDevice::SetHidData(HidDataPtr& ptr, const unsigned long offset, const USAGE up, const USAGE usage, const unsigned long rid)
{
    // TODO Should verify offset is within range.
    (ptr.get()[offset]).IsButtonData = false;
    (ptr.get()[offset]).Status = HIDP_STATUS_SUCCESS;
    (ptr.get()[offset]).UsagePage = up;
    (ptr.get()[offset]).ValueData.Usage = usage;
    (ptr.get()[offset]).ReportID = rid;
}

/// <summary>
/// Attempts to start a read
/// </summary>
/// <param name="overlap">unique_ptr to overlap structure</param>
/// <returns>true if read successful / false otherwise</returns>
bool HidDevice::ReadOverlapped(std::unique_ptr<OVERLAPPED>& overlap)
{
    bool readStatus{ false };
    unsigned long bytesRead{ 0 };

    // Execute the read and save return code to determine how to proceed
    readStatus = ReadFile(device,
                          InputReportBuffer.get(),
                          Caps->InputReportByteLength,
                          &bytesRead,
                          overlap.get());

    // If readStatus is false, then one of two cases occurred:
    // 1. Readfile call succeeded, but the read is an overlapped one. Here,
    //      return true to indicate read successful. However, the calling
    //      thread should be blocked on the completion event which means
    //      it will continue until the read actually completes.
    // 2. The readfile call failed for some unknown reason. In this case,
    //      return code will be false
    if (!readStatus)
    {
        return (GetLastError() == ERROR_IO_PENDING);
    }
    // If readStatus is true, then the Readfile call completed synchronously.
    //   Signal the completion event so the calling thread knows it can continue
    else
    {
        SetEvent(overlap->hEvent);
        return true;
    }
}

/// <summary>
/// Thread procedure for asynchronous read. Verifies device opened with appropriate
/// permissions. Continues to read until either:
/// a. terminateThread is set to true in the controller thread
/// b. maxCharToRead is not UINT_MAX AND we have not read this many characters
/// </summary>
/// <param name="maxCharToRead">Maximum number of characters to read</param>
/// <param name="terminateThread">Flag for thread termination. Set to true by controlling
/// thread when thread should exit.</param>
/// <returns>true if read successful / false otherwise</returns>
bool HidDevice::ReadAsyncThreadProc(unsigned int maxCharToRead, std::atomic<bool>& terminateThread)
{
    // Verify device is open for read and overlapped.
    if (!IsOpen() && OpenedForRead && OpenedOverlapped)
    {
        // If not, reopen with correct permissions
        Open(true, false, true);
        if (!IsOpen() && OpenedForRead && OpenedOverlapped)
        {
            // Still not open, throw an exception
            throw std::runtime_error("HidDevice cannot be opened for async read.");
        }
    }

    // Create completion event for read
    HANDLE completionEvent = CreateEventA(nullptr, false, false, nullptr);
    if (completionEvent == nullptr) { return false; }

    bool readResult{ false };
    unsigned long numReadsDone{ 0 };
    unsigned long waitStatus;
    unsigned long bytesTransferred{ 0 };

    // loop for reading until termination condition or reaching maxCharToRead
    do
    {
        // Create a unique_ptr to an OVERLAPPED structure. This is a unique_ptr
        // so it's scope is only for a single loop. That way it gets recreated
        // and rezeroed each time through the loop.
        std::unique_ptr<OVERLAPPED> overlap =
            std::make_unique<OVERLAPPED>(OVERLAPPED{ .hEvent = completionEvent });

        // Perform an overlapped read
        readResult = ReadOverlapped(overlap);

        if (!readResult) { return false; }

        // Wait for the completion of the read or termination signal
        while (!terminateThread.load())
        {
            waitStatus = WaitForSingleObject(completionEvent, READ_THREAD_TIMEOUT);

            if (waitStatus == WAIT_OBJECT_0)
            {
                // Retrieve the result of the overlapped read operation 
                readResult = GetOverlappedResult(device, overlap.get(), &bytesTransferred, true);
                break;
            }
        }

        // If termination signal not received and read successful, process the input data.
        if (!terminateThread.load())
        {
            numReadsDone++;

            UnpackReport(InputReportBuffer,
                         Caps->InputReportByteLength,
                         HidP_Input,
                         InputData,
                         InputDataLength,
                         Ppd);
        }

    } while (readResult &&
             !terminateThread.load() &&
             (maxCharToRead == UINT_MAX || numReadsDone < maxCharToRead));

    return true;
}

/// <summary>
/// Creates a new HidDevice from the DevicePath
/// </summary>
/// <param name="DevicePath">The Device Path</param>
HidDevice::HidDevice(const std::string& DevicePath)
{
    this->DevicePath = DevicePath;
    this->device = INVALID_HANDLE_VALUE;
    this->Ppd = std::make_unique<PHIDP_PREPARSED_DATA>();
    this->Attributes = std::make_unique<HIDD_ATTRIBUTES>();
    this->Caps = std::make_unique<HIDP_CAPS>();
}

/// <summary>
/// HidDevice destructor. Closes handle if opened.
/// </summary>
HidDevice::~HidDevice() { Close(); }

/// <summary>
/// Closes HidDevice handle. Sets handle to invalid handle value and resets opened flags.
/// </summary>
void HidDevice::Close()
{
    if (IsOpen())
    {
        CloseHandle(device);
        device = INVALID_HANDLE_VALUE;
        OpenedForRead = OpenedForWrite = OpenedExclusive = OpenedOverlapped = false;
    }
}

/// <summary>
/// Checks if HidDevice handle is an invalid value indicating device closed.
/// </summary>
/// <returns>true if handle is valid / false otherwise</returns>
bool HidDevice::IsOpen() const
{
    return device != INVALID_HANDLE_VALUE;
}


/// <summary>
/// Synchronous read of HidDevice object. Checks if open and if is not open,
/// reopens with read permissions. If cannot open, throws a runtime_error exception.
/// The read data is inserted into the InputData buffer.
/// </summary>
/// <exception cref="std::runtime_error">Thrown if HidDevice cannot be opened</exception>
/// <returns>true if read was successful / false otherwise</returns>
bool HidDevice::Read()
{
    unsigned long bytesRead;

    if (!IsOpen() && OpenedForRead)
    {
        Open(true);
        if (!IsOpen() && OpenedForRead)
        {
            throw std::runtime_error("HidDevice cannot be opened for read.");
        }
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

// This is necessary to facilitate using a lambda function with capture for
// the signal handler. 
namespace
{
    std::function<void(int)> terminateHandler;
    void signalHandler(int signal)
    {
        if (terminateHandler)
        {
            terminateHandler(signal);
        }
    }
}

/// <summary>
/// Asynchronous read of HidDevice object. Spawns a new thread for the asynchronous read
/// operation and enables Control-C interrupt of read.
/// </summary>
/// <param name="maxCharToRead">Number of characters to read from device</param>
/// <returns>true if read successful / false otherwise</returns>
bool HidDevice::ReadAsync(unsigned int maxCharToRead)
{
    std::atomic<bool> success(false);
    std::atomic<bool> terminateThread(false);

    // Set the SIGINT handler to set the terminateThread atomic to true
    terminateHandler = [&terminateThread](int)
        {
            terminateThread.store(true);
        };
    std::signal(SIGINT, signalHandler);

    Open(true, false, true, false);     // Open for read and overlapped use

    std::jthread readThread([&success, this, maxCharToRead, &terminateThread]()
                            {
                                // Spawn thread and store result in success atomic variable.
                                success.store(ReadAsyncThreadProc(maxCharToRead, terminateThread));
                            });

    // Wait for thread to exit. Control-C will set terminateThread to true which will
    // force the thread to exit without finishing the read.
    readThread.join();
    std::signal(SIGINT, SIG_DFL);   // Reset the SIGINT handler
    return success.load();
}

/// <summary>
/// Attempts to open the HidDevice with the chosen permissions.
/// If HidDevice is already open, will close the device first before attempting
/// to reopen with the specified permissions
/// </summary>
/// <param name="HasReadAccess">Open with read access</param>
/// <param name="HasWriteAccess">Open with write access (NOT IMPLEMENTED)</param>
/// <param name="IsOverlapped">Open for asynchronous operation</param>
/// <param name="IsExclusive">Carryover from hclient sample. Not sure what this does. (NOT IMPLEMENTED)</param>
/// <returns>true if HidDevice opened without issue / false otherwise</returns>
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
    if (IsOverlapped)
    {
        CloseHandle(device);
        device = INVALID_HANDLE_VALUE;
        device = CreateFileA(DevicePath.c_str(),
                             accessFlags,
                             sharingFlags,
                             nullptr,
                             OPEN_EXISTING,
                             FILE_FLAG_OVERLAPPED,
                             nullptr);
        if (device == INVALID_HANDLE_VALUE) { return false; }
    }

    return true;
}

/// <summary>
/// Fills HidDevice class with data from the system
/// </summary>
/// <returns>true on success / false on any errors</returns>
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

/// <summary>
/// Initializes a blank HidDevices object. Clears the devices vector.
/// </summary>
HidDevices::HidDevices() { devices.clear(); }

/// <summary>
/// Gets the size of the devices vector
/// </summary>
/// <returns>The current size of the devices vector</returns>
size_t HidDevices::size() { return devices.size(); }

/// <summary>
/// Finds all HID devices that can be opened on the system. Adds them all to the devices vector.
/// </summary>
/// <param name="bool CloseAllDevices">If true, closes all devices after discovery</param>
/// <returns>true if success/false if any errors encountered</returns>
bool HidDevices::FindAllHidDevices(bool CloseAllDevices)
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
                if (hidDeviceInst->IsOpen())
                {
                    // Add new HidDevice to devices vector
                    devices.push_back(std::move(hidDeviceInst));
                }
            }
        }
        i++;
    } while (GetLastError() != ERROR_NO_MORE_ITEMS);

    if (CloseAllDevices)
    {
        for (const auto& dev : devices)
        {
            dev.get()->Close();
        }
    }

    return true;
}

/// <summary>
/// Retrieves the device list
/// </summary>
/// <returns>Reference to a vector containing unique_ptrs to each HidDevice</returns>
std::vector<HidDevicePtr>& HidDevices::getDevices()
{
    return devices;
}

/// <summary>
/// Retrieves the device list (constant)
/// </summary>
/// <returns>constant Reference to a vector containing unique_ptrs to each HidDevice</returns>
const std::vector<HidDevicePtr>& HidDevices::getDevices() const
{
    return devices;
}

/// <summary>
/// Subscript operator for HidDevices
/// </summary>
/// <param name="idx">Index to retrieve</param>
/// <exception cref="std::out_of_range">Thrown if index is out of range of available devices</exception>
/// <returns>Unique_ptr to HidDevice in device list at specified index</returns>
std::unique_ptr<HidDevice> HidDevices::operator[](int idx)
{
    if (idx < 0 || idx >= devices.size())
    {
        throw std::out_of_range("HidDevices[]: out of range");
    }
    return std::move(devices.at(idx));
}

std::ostream& operator<<(std::ostream& strm, const HidDevice& hd)
{

    strm << std::format("{:<50} VID: {:#06x} PID: {:#06x} UsagePage: {:#04x} Usage: {:#04x}\n",
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
