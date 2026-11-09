#pragma once
#include <Windows.h>
#include <string>
#include<filesystem>
#include"hash.hpp"


enum class RETURN {
    FILE_ALREADY_SIGNED,
    SIGN_SUCCESS,
    INVALID_FILE
};

namespace mdata{ //metadata handling

    bool WriteId(const std::string& filename, const std::string& alt_stream, const std::string& id){
        std::string stream = filename + ":" + alt_stream;
        HANDLE file = CreateFileA(stream.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
        if(file==INVALID_HANDLE_VALUE) return false;
        DWORD written;
        bool success = WriteFile(file, id.data(), static_cast<DWORD>(id.size()), &written, nullptr);
        CloseHandle(file);
        return success && written == id.size();
    }

    std::string ReadId(const std::string& filename, const std::string& alt_stream){
        std::string stream = filename + ":" + alt_stream;

        HANDLE file = CreateFileA(stream.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
        if(file==INVALID_HANDLE_VALUE) 	return "UNSIGNED"; //ALTERNATE STREAM DOESNT EXIST

        char buffer[50] = {}; //for now can only up to 50 chars, if any more than that were fucked. ill fix later
        DWORD bytes_read;
        bool success = ReadFile(file, buffer, sizeof(buffer), &bytes_read, nullptr);
        CloseHandle(file);
        if(!success) return "UNSIGNED"; // ERROR READING ALTERNATE STREAM
        return std::string(buffer,bytes_read);

    }

    RETURN SignFile(std::filesystem::path path, std::string name, std::string alt_stream, bool force_sign = false){
        std::string hash = nhash::SHA256(name);

        if(ReadId(path.string(),alt_stream)!="UNSIGNED" && !force_sign)
            return RETURN::FILE_ALREADY_SIGNED;
        
        if(!WriteId(path.string(), alt_stream, hash))
            return RETURN::INVALID_FILE;
        
        return RETURN::SIGN_SUCCESS;
    }
}