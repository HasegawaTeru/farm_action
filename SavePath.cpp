#include "SavePath.h"

#include <vector>

#ifdef _WIN32
#include <windows.h>
#endif

std::wstring DefaultSaveDirectory()
{
#ifdef _WIN32
    for (DWORD capacity = 260; ; )
    {
        std::vector<wchar_t> buffer(capacity);

        const DWORD length = GetModuleFileNameW(
            nullptr,
            buffer.data(),
            capacity
        );

        if (length == 0)
        {
            return L"";
        }

        if (length < capacity)
        {
            const std::wstring executable(
                buffer.data(),
                length
            );

            const auto slash =
                executable.find_last_of(L"\\/");

            if (slash == std::wstring::npos)
            {
                return L"";
            }

            return executable.substr(0, slash + 1)
                + L"SaveData";
        }

        if (capacity == 32768)
        {
            return L"";
        }

        capacity =
            capacity * 2 > 32768
            ? 32768
            : capacity * 2;
    }
#endif

    return L"";
}