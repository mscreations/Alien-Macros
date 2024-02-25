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
    if (RegOpenKeyExA(HKEY_CURRENT_USER, GetFullPath(subKey).c_str(), 0, KEY_READ, &hSubKey) == ERROR_SUCCESS)
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
            subKeyName.resize(subkeySize);  // resize string to size returned by function
            subItems.push_back(subKeyName); // move string to vector
            idx++;
            subkeySize = MAX_PATH;
        } while (result == ERROR_SUCCESS);

        RegCloseKey(hSubKey);
    }
    return subItems;
}

/// <summary>
/// Adds subKey to the appSubKey if it exists
/// </summary>
/// <param name="subKey">subKey to be added to appSubKey</param>
/// <returns>The full path</returns>
std::string RegistryHelper::GetFullPath(const std::string& subKey)
{
    std::string fullPath = appSubKey;
    if (subKey.length() > 0)
    {
        fullPath += "\\" + subKey;
    }
    return fullPath;
}

/// <summary>
/// Writes std::string value to specified valueName
/// </summary>
/// <param name="valueName">Value name to save</param>
/// <param name="valueData">Data to save to valueName</param>
/// <returns>true if successful / false otherwise</returns>
bool RegistryHelper::WriteStringValue(const std::string& valueName, const std::string& valueData, const std::string& subKey)
{
    return RegSetKeyValueA(HKEY_CURRENT_USER, GetFullPath(subKey).c_str(), valueName.c_str(), REG_SZ, valueData.c_str(), static_cast<unsigned long>((valueData.length() + 1) * sizeof(char))) == ERROR_SUCCESS;
}

/// <summary>
/// Reads string value from Windows registry
/// </summary>
/// <param name="valueName">Value to retrieve</param>
/// <param name="valueData">std::string reference that will contain the value in the registry</param>
/// <returns>true on success / false otherwise</returns>
bool RegistryHelper::ReadStringValue(const std::string& valueName, std::string& valueData, const std::string& subKey)
{
    unsigned long bufferSize{ 0 };

    long result = RegGetValueA(HKEY_CURRENT_USER, GetFullPath(subKey).c_str(), valueName.c_str(), RRF_RT_REG_SZ, nullptr, nullptr, &bufferSize);

    if (result == ERROR_SUCCESS)
    {
        valueData.assign(bufferSize, 0);
        result = RegGetValueA(HKEY_CURRENT_USER, GetFullPath(subKey).c_str(), valueName.c_str(), RRF_RT_REG_SZ, nullptr, valueData.data(), &bufferSize);

        valueData.resize(bufferSize - 1);

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
bool RegistryHelper::WriteDwordValue(const std::string& valueName, const unsigned long& valueData, const std::string& subKey)
{
    return RegSetKeyValueA(HKEY_CURRENT_USER, GetFullPath(subKey).c_str(), valueName.c_str(), REG_DWORD, &valueData, sizeof(unsigned long)) == ERROR_SUCCESS;
}

/// <summary>
/// Reads Dword value from Windows registry
/// </summary>
/// <param name="valueName">Value to read</param>
/// <param name="valueData">Referenced var to receive data</param>
/// <returns>true on success / false otherwise</returns>
bool RegistryHelper::ReadDwordValue(const std::string& valueName, unsigned long& valueData, const std::string& subKey)
{
    unsigned long dataType, dataSize = sizeof(unsigned long);
    long result = RegGetValueA(HKEY_CURRENT_USER, GetFullPath(subKey).c_str(), valueName.c_str(), RRF_RT_REG_DWORD, &dataType, &valueData, &dataSize);
    return result == ERROR_SUCCESS && dataType == REG_DWORD;
}

bool RegistryHelper::CheckIfKeyExists(const std::string& keyName, const std::string& subKey)
{
    HKEY hKey;
    std::string key = GetFullPath(subKey) + "\\" + keyName;
    long result = RegOpenKeyExA(HKEY_CURRENT_USER, key.c_str(), 0, KEY_READ, &hKey);
    if (result == ERROR_SUCCESS)
    {
        RegCloseKey(hKey);
        return true;
    }
    return false;
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

