#pragma once
#include <Windows.h>
#include <string>

namespace mdata{ //metadata handling
    bool WriteId(const std::wstring& filename, const std::wstring& alt_stream, const std::wstring& id){
        std::wstring stream = filename + L":" + alt_stream;
        HANDLE file = CreateFileW(stream.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
        if(file==INVALID_HANDLE_VALUE) return false;
        DWORD written;
        bool success = WriteFile(file, id.c_str(), static_cast<DWORD>(id.size()*sizeof(wchar_t)), &written, nullptr);
        CloseHandle(file);
        return success && written == id.size() * sizeof(wchar_t);
    }

    std::wstring ReadId(const std::wstring& filename, const std::wstring& alt_stream){
        std::wstring stream = filename + L":" + alt_stream;

        HANDLE file = CreateFileW(stream.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
        if(file==INVALID_HANDLE_VALUE) return L"";

        wchar_t buffer[50] = {}; //for now can only up to 50 chars, if any more than that were fucked. ill fix later
        DWORD bytes_read;
        bool success = ReadFile(file, buffer, sizeof(buffer) - sizeof(wchar_t), &bytes_read, nullptr);
        CloseHandle(file);
        if(!success) return L"";
        return buffer;

    }
}