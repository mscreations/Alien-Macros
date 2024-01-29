#include <format>
#include <iostream>
#include "TargetDevice.h"

/// <summary>
/// Default constructor. Initializes all members to 0.
/// </summary>
TargetDevice::TargetDevice() : targetVID{ 0 }, targetPID{ 0 }, usagePage{ 0 }, usageCode{ 0 } {}

/// <summary>
/// int Constructor. Casts all parameters to unsigned short
/// </summary>
/// <param name="vid">Vendor ID</param>
/// <param name="pid">Product ID</param>
/// <param name="up">Usage Page</param>
/// <param name="uc">Usage Code</param>
TargetDevice::TargetDevice(int vid, int pid, int up, int uc) :
    targetVID{ static_cast<unsigned short>(vid) },
    targetPID{ static_cast<unsigned short>(pid) },
    usagePage{ static_cast<unsigned short>(up) },
    usageCode{ static_cast<unsigned short>(uc) }
{
}

/// <summary>
/// Checks two TargetDevice for equality
/// </summary>
/// <param name="other">2nd object to compare</param>
/// <returns>true if objects are equal / false otherwise</returns>
bool TargetDevice::operator==(const TargetDevice& other) const
{
    return targetVID == other.targetVID &&
        targetPID == other.targetPID &&
        usagePage == other.usagePage &&
        usageCode == other.usageCode;
}

/// <summary>
/// Output stream operator for TargetDevice object. Formats 
/// </summary>
/// <param name="strm">Stream object to write to</param>
/// <param name="dev">TargetDevice to output data from</param>
/// <returns>Stream object written to</returns>
std::ostream& operator<<(std::ostream& strm, const TargetDevice& dev)
{
    return strm << std::format("VID:       {:#06x}\nPID:       {:#06x}\nUsagePage: {:#04x}\nUsageCode: {:#04x}",
                        dev.targetVID, dev.targetPID, dev.usagePage, dev.usageCode);
}

/// <summary>
/// Hashing function used for unordered_set hash
/// </summary>
/// <param name="device">TargetDevice object to calculate hash</param>
/// <returns>Hash of TargetDevice object</returns>
std::size_t TargetDevice::HashFunction::operator()(const TargetDevice& device) const
{
    return std::hash<unsigned short>()(device.targetVID) ^
        std::hash<unsigned short>()(device.targetPID) ^
        std::hash<unsigned short>()(device.usagePage) ^
        std::hash<unsigned short>()(device.usageCode);
}
