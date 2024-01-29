#include "RegistryHelper.h"
#include <Windows.h>

/// <summary>
/// Enumerates subkey for keys/values
/// </summary>
/// <param name="subKey">Subkey to enumerate</param>
/// <param name="GetValues">true if should retrieve values / false to retrieve keys</param>
/// <returns>std::vector of std::string key/value names</returns>
std::vector<std::string> RegistryHelper::GenericEnumerate(const std::string& subKey, const bool GetValues)
{
    std::vector<std::string> subItems{};

    HKEY hSubKey;
    if (RegOpenKeyExA(HKEY_CURRENT_USER, subKey.c_str(), 0, KEY_READ, &hSubKey) == ERROR_SUCCESS)
    {
        unsigned long idx{ 0 };
        std::string subKeyName(MAX_PATH, 0);
        unsigned long subkeySize = MAX_PATH;

        long result;

        do
        {
            result = (GetValues ?
                           RegEnumValueA(hSubKey, idx, subKeyName.data(), &subkeySize, nullptr, nullptr, nullptr, nullptr) :
                           RegEnumKeyExA(hSubKey, idx, subKeyName.data(), &subkeySize, nullptr, nullptr, nullptr, nullptr));
            if (result == ERROR_NO_MORE_ITEMS) { break; }
            subItems.push_back(subKeyName);
            idx++;
            subkeySize = MAX_PATH;
        } while (result == ERROR_SUCCESS);

        RegCloseKey(hSubKey);
    }
    return subItems;
}

/// <summary>
/// Writes std::string value to specified valueName
/// </summary>
/// <param name="valueName">Value name to save</param>
/// <param name="valueData">Data to save to valueName</param>
/// <returns>true if successful / false otherwise</returns>
bool RegistryHelper::WriteStringValue(const std::string& valueName, const std::string& valueData)
{
    return RegSetKeyValueA(HKEY_CURRENT_USER, appSubKey.c_str(), valueName.c_str(), REG_SZ, valueData.c_str(), static_cast<unsigned long>((valueData.length() + 1) * sizeof(char))) == ERROR_SUCCESS;
}

/// <summary>
/// Reads string value from Windows registry
/// </summary>
/// <param name="valueName">Value to retrieve</param>
/// <param name="valueData">std::string reference that will contain the value in the registry</param>
/// <returns>true on success / false otherwise</returns>
bool RegistryHelper::ReadStringValue(const std::string& valueName, std::string& valueData)
{
    unsigned long bufferSize{ 0 };

    long result = RegGetValueA(HKEY_CURRENT_USER, appSubKey.c_str(), valueName.c_str(), RRF_RT_REG_SZ, nullptr, nullptr, &bufferSize);

    if (result == ERROR_SUCCESS)
    {
        valueData.assign(bufferSize, 0);
        result = RegGetValueA(HKEY_CURRENT_USER, appSubKey.c_str(), valueName.c_str(), RRF_RT_REG_SZ, nullptr, valueData.data(), &bufferSize);

        if (result != ERROR_SUCCESS) { valueData.clear(); }
        return result == ERROR_SUCCESS;
    }
    return false;
}

/// <summary>
/// Writes Dword value to Windows registry
/// </summary>
/// <param name="valueName">ValueName to save to</param>
/// <param name="valueData">Data to be saved</param>
/// <returns>true on success / false otherwise</returns>
bool RegistryHelper::WriteDwordValue(const std::string& valueName, const unsigned long& valueData)
{
    return RegSetKeyValueA(HKEY_CURRENT_USER, appSubKey.c_str(), valueName.c_str(), REG_DWORD, &valueData, sizeof(unsigned long)) == ERROR_SUCCESS;
}

/// <summary>
/// Reads Dword value from Windows registry
/// </summary>
/// <param name="valueName">Value to read</param>
/// <param name="valueData">Referenced var to receive data</param>
/// <returns>true on success / false otherwise</returns>
bool RegistryHelper::ReadDwordValue(const std::string& valueName, unsigned long& valueData)
{
    unsigned long dataType, dataSize = sizeof(unsigned long);
    long result = RegGetValueA(HKEY_CURRENT_USER, appSubKey.c_str(), valueName.c_str(), RRF_RT_REG_DWORD, &dataType, &valueData, &dataSize);
    return result == ERROR_SUCCESS && dataType == REG_DWORD;
}

/// <summary>
/// Enumerates All the subkeys of the specified subKey
/// </summary>
/// <param name="subKey">subKey to enumerate</param>
/// <returns>std::vector of std::string with all the subkey names</returns>
std::vector<std::string> RegistryHelper::EnumerateSubKeys(const std::string& subKey)
{
    return GenericEnumerate(subKey);
}

/// <summary>
/// Enumerates the given subkey of all Value Names
/// </summary>
/// <param name="subKey">subkey to enumerate</param>
/// <returns>std::vector of std::string value names</returns>
std::vector<std::string> RegistryHelper::EnumerateSubValues(const std::string& subKey)
{
    return GenericEnumerate(subKey, true);
}

