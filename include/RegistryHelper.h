#pragma once
#include <string>
#include <vector>

/// <summary>
/// Class that provides friendly access to Windows registry
/// </summary>
class RegistryHelper
{
    inline static const std::string appSubKey = "SOFTWARE\\Alien-Macros";
    static std::vector<std::string> GenericEnumerate(const std::string& subKey, const bool GetValues = false);
    static std::string GetFullPath(const std::string& subKey);
public:
    [[nodiscard]] static bool WriteStringValue(const std::string& valueName, const std::string& valueData, const std::string& subKey = "");
    [[nodiscard]] static bool ReadStringValue(const std::string& valueName, std::string& valueData, const std::string& subKey = "");

    [[nodiscard]] static bool WriteDwordValue(const std::string& valueName, const unsigned long& valueData, const std::string& subKey = "");
    [[nodiscard]] static bool ReadDwordValue(const std::string& valueName, unsigned long& valueData, const std::string& subKey = "");

    [[nodiscard]] static bool CheckIfKeyExists(const std::string& keyName, const std::string& subKey = "");
    static std::vector<std::string> EnumerateSubKeys(const std::string& subKey = "");
    static std::vector<std::string> EnumerateSubValues(const std::string& subKey = "");
};

