#pragma once
#include <unordered_set>

struct TargetDevice
{
    unsigned short targetVID;
    unsigned short targetPID;
    unsigned short usagePage;
    unsigned short usageCode;
    TargetDevice() : targetVID{ 0 }, targetPID{ 0 }, usagePage{ 0 }, usageCode{ 0 } {}
    TargetDevice(int vid, int pid, int up, int uc) :
        targetVID{ static_cast<unsigned short>(vid) },
        targetPID{ static_cast<unsigned short>(pid) },
        usagePage{ static_cast<unsigned short>(up) },
        usageCode{ static_cast<unsigned short>(uc) }
    {
    }

    struct HashFunction
    {
        std::size_t operator()(const TargetDevice& device) const
        {
            return std::hash<unsigned short>()(device.targetVID) ^
                std::hash<unsigned short>()(device.targetPID) ^
                std::hash<unsigned short>()(device.usagePage) ^
                std::hash<unsigned short>()(device.usageCode);
        }
    };

    bool operator==(const TargetDevice& other) const
    {
        return targetVID == other.targetVID &&
            targetPID == other.targetPID &&
            usagePage == other.usagePage &&
            usageCode == other.usageCode;
    }
};
