#pragma once

/// <summary>
/// TargetDevice class. Represents a target device.
/// </summary>
struct TargetDevice
{
    unsigned short targetVID;
    unsigned short targetPID;
    unsigned short usagePage;
    unsigned short usageCode;
    TargetDevice();
    TargetDevice(int vid, int pid, int up, int uc);

    struct HashFunction
    {
        std::size_t operator()(const TargetDevice& device) const;
    };

    bool operator==(const TargetDevice& other) const;
    friend std::ostream& operator<<(std::ostream& strm, const TargetDevice& dev);
};
