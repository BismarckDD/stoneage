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

inline std::wstring Utf8ToWide(const char *text)
{
    if (text == NULL || text[0] == '\0')
        return std::wstring();

    int length = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, text, -1,
                                     NULL, 0);
    if (length <= 0)
        return std::wstring();

    std::wstring result(length, L'\0');
    if (MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, text, -1,
                            &result[0], length) <= 0)
        return std::wstring();
    result.resize(length - 1);
    return result;
}

inline std::string Utf8ToGbk(const char *text)
{
    if (text == NULL || text[0] == '\0')
        return std::string();

    int wideLength = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, text, -1,
                                         NULL, 0);
    if (wideLength <= 0)
        return std::string();

    std::wstring wideText(wideLength, L'\0');
    MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, text, -1,
                        &wideText[0], wideLength);

    int gbkLength = WideCharToMultiByte(936, 0, wideText.c_str(), -1, NULL, 0,
                                        NULL, NULL);
    if (gbkLength <= 0)
        return std::string();

    std::string result(gbkLength, '\0');
    WideCharToMultiByte(936, 0, wideText.c_str(), -1, &result[0], gbkLength,
                        NULL, NULL);
    result.resize(gbkLength - 1);
    return result;
}

inline std::string GbkToUtf8(const char *text)
{
    if (text == NULL || text[0] == '\0')
        return std::string();

    int wideLength = MultiByteToWideChar(936, MB_ERR_INVALID_CHARS, text, -1,
                                         NULL, 0);
    if (wideLength <= 0)
        return std::string();

    std::wstring wideText(wideLength, L'\0');
    if (MultiByteToWideChar(936, MB_ERR_INVALID_CHARS, text, -1,
                            &wideText[0], wideLength) <= 0)
        return std::string();

    int utf8Length = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS,
                                         wideText.c_str(), -1, NULL, 0,
                                         NULL, NULL);
    if (utf8Length <= 0)
        return std::string();

    std::string result(utf8Length, '\0');
    if (WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS,
                            wideText.c_str(), -1, &result[0], utf8Length,
                            NULL, NULL) <= 0)
        return std::string();

    result.resize(utf8Length - 1);
    return result;
}

inline BOOL SetWindowTextGbk(HWND window, const char *text)
{
    const std::wstring wideText = GbkToWide(text);
    return SetWindowTextW(window, wideText.c_str());
}

inline BOOL SetWindowTextUtf8(HWND window, const char *text)
{
    const std::wstring wideText = Utf8ToWide(text);
    return SetWindowTextW(window, wideText.c_str());
}

inline int MessageBoxGbk(HWND window, const char *text, const char *caption, UINT type)
{
    const std::wstring wideText = GbkToWide(text);
    const std::wstring wideCaption = GbkToWide(caption);
    return MessageBoxW(window, wideText.c_str(), wideCaption.c_str(), type);
}

inline int MessageBoxUtf8(HWND window, const char *text, const char *caption,
                          UINT type)
{
    const std::wstring wideText = Utf8ToWide(text);
    const std::wstring wideCaption = Utf8ToWide(caption);
    return MessageBoxW(window, wideText.c_str(), wideCaption.c_str(), type);
}
