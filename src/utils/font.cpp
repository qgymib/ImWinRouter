#include <windows.h>
#include <stdexcept>
#include <map>
#include "utils/string.hpp"
#include "font.hpp"

typedef std::map<std::wstring, std::wstring> FontMap;

static std::wstring GetDefaultSystemFontName()
{
    NONCLIENTMETRICSW ncm;
    ZeroMemory(&ncm, sizeof(ncm));
    ncm.cbSize = sizeof(NONCLIENTMETRICSW);

    // Get the system's default GUI font
    if (SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof(ncm), &ncm, 0))
    {
        return ncm.lfMessageFont.lfFaceName;
    }

    throw std::runtime_error("Failed to get default system font");
}

static FontMap GetFontMap()
{
    FontMap fontMap;

    HKEY    hKey;
    LSTATUS result =
        RegOpenKeyExW(HKEY_LOCAL_MACHINE,
                      L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Fonts",
                      0, KEY_READ, &hKey);
    if (result != ERROR_SUCCESS)
    {
        throw std::runtime_error("Failed to open registry key");
    }

    WCHAR valueName[MAX_PATH];
    BYTE  data[MAX_PATH];
    DWORD index = 0;
    DWORD type;

    while (true)
    {
        DWORD valueNameSize = sizeof(valueName);
        DWORD dataSize = sizeof(data);

        result = RegEnumValueW(hKey, index, valueName, &valueNameSize, nullptr,
                               &type, data, &dataSize);
        if (result != ERROR_SUCCESS)
        {
            break;
        }

        if (type != REG_SZ)
        {
            continue;
        }

        fontMap.insert(FontMap::value_type(valueName, (WCHAR*)data));
        index++;
    }

    RegCloseKey(hKey);

    return fontMap;
}

std::string iwr::GetDefaultFontPath()
{
    std::wstring defaultName = GetDefaultSystemFontName();
    FontMap      fontMap = GetFontMap();

    for (const auto& font : fontMap)
    {
        std::wstring fontName = font.first;
        if (fontName.find(defaultName) != std::wstring::npos)
        {
            // Construct full font file path
            WCHAR windowsDir[MAX_PATH] = { 0 };
            GetWindowsDirectoryW(windowsDir, MAX_PATH);
            std::wstring full_path =
                std::wstring(windowsDir) + L"\\Fonts\\" + font.second;
            return iwr::ToString(full_path);
        }
    }

    throw std::runtime_error("Failed to get default system font");
}
