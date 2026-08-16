#pragma once

#include <windows.h>
#include <string>

// The legacy game protocol and data files use Simplified Chinese GBK (CP936).
// Convert only at the Win32 boundary so behavior does not depend on system ACP.
inline std::wstring GbkToWide(const char *text)
{
    if (text == NULL || text[0] == '\0')
        return std::wstring();

    int length = MultiByteToWideChar(936, 0, text, -1, NULL, 0);
    if (length <= 0)
        return std::wstring();

    std::wstring result(length, L'\0');
    MultiByteToWideChar(936, 0, text, -1, &result[0], length);
    result.resize(length - 1);
    return result;
}

inline BOOL SetWindowTextGbk(HWND window, const char *text)
{
    const std::wstring wideText = GbkToWide(text);
    return SetWindowTextW(window, wideText.c_str());
}

inline int MessageBoxGbk(HWND window, const char *text, const char *caption, UINT type)
{
    const std::wstring wideText = GbkToWide(text);
    const std::wstring wideCaption = GbkToWide(caption);
    return MessageBoxW(window, wideText.c_str(), wideCaption.c_str(), type);
}
