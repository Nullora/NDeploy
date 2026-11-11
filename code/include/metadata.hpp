#pragma once
#include <Windows.h>
#include <string>
#include<filesystem>
#include"hash.hpp"


enum class RETURN {
    FILE_ALREADY_SIGNED,
    SIGN_SUCCESS,
    INVALID_FILE,
    INVALID_NAME
};

struct FileRep{
    RETURN last_return;
    std::string id;
    std::string content_hash;
    std::string copy_id;
    std::string version_id;
    std::filesystem::path og_path;
};

namespace mdata{ //metadata handling

    bool WriteMData(const std::string& filename, const std::string& alt_stream, const std::string& stream_sector,const std::string& data){
        std::string stream = filename + ":" + alt_stream + "_" + stream_sector;
        HANDLE file = CreateFileA(stream.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr); 
        if(file==INVALID_HANDLE_VALUE) return false;
        DWORD written;
        bool success = WriteFile(file, data.data(), static_cast<DWORD>(data.size()), &written, nullptr);
        CloseHandle(file);
        return success && written == data.size();
    }

    std::string ReadMData(const std::string& filename, const std::string& alt_stream, const std::string& stream_sector){
        std::string stream = filename + ":" + alt_stream + "_" + stream_sector;

        HANDLE file = CreateFileA(stream.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
        if(file==INVALID_HANDLE_VALUE) 	return "UNSIGNED"; //ALTERNATE STREAM DOESNT EXIST

        char buffer[50] = {}; //for now can only up to 50 chars, if any more than that were fucked. ill fix later
        DWORD bytes_read;
        bool success = ReadFile(file, buffer, sizeof(buffer), &bytes_read, nullptr);
        CloseHandle(file);
        if(!success) return "UNSIGNED"; // ERROR READING ALTERNATE STREAM
        return std::string(buffer,bytes_read);

    }

    FileRep SignFile(std::filesystem::path path, std::string name, std::string alt_stream, bool force_sign = false){
        FileRep lib;
        //check if file is already signed
        if(ReadMData(path.string(),alt_stream,"ID")!="UNSIGNED" && !force_sign){
            lib.last_return = RETURN::FILE_ALREADY_SIGNED;
            return lib;
        }
        //check if name contains :
        if(name.find(":") != std::string::npos){
            lib.last_return = RETURN::INVALID_NAME;
            return lib;
        }


        //write id. used to determine identity of file disregarding filename or location
        //ill check for stored names later cz rn 2 same names can exist
        if(!WriteMData(path.string(), alt_stream, "ID", name)){
            lib.last_return = RETURN::INVALID_FILE;
            return lib;
        }

        //write content hash. used to compare 2 files of the same identity but imported in different projects. useful when `ndep update` is used to not cause conflict between both versions
        std::ifstream file(path, std::ios::binary);
        std::vector<unsigned char> bytes(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
        std::string content_hash = nhash::SHA256File(path);
        WriteMData(path.string(), alt_stream, "CONTENT_HASH", content_hash);

        //write copy_id. copy_id tracks the location of the current file.
        //when `ndep sign` is used it leaves the original file's copy_id = og_file. it changes the repository's version of that file's copy_id to rep_file
        //when `ndep import` is used, it changes copy_id to a hash of the directory where the library was imported. it also adds the hash to a database to track all distributed copies
        WriteMData(path.string(), alt_stream, "COPY_ID", "OG_LIB");

        //write version_id. version_id tracks the official version of the file assigned by ndep.
        WriteMData(path.string(), alt_stream, "VERSION_ID", "1.0.0");

        lib.id = name;
        lib.content_hash = content_hash;
        lib.copy_id = "OG_LIB";
        lib.version_id = "1.0.0";
        lib.og_path = path;
        lib.last_return = RETURN::SIGN_SUCCESS;

        return lib;
    }

    bool UnsignFile(const std::string path, const std::string& alt_stream){
        std::vector<std::string> stream_sectors = {"ID", "CONTENT_HASH", "COPY_ID", "VERSION_ID"};
        std::string stream;
        bool state = false;
        for(auto& sector : stream_sectors){
            stream = path + ":" + alt_stream + "_" +sector;
            state = DeleteFileA(stream.c_str()); //ik this only applies to the last sector deleted, but signing a file means all the sectors were written
            //so proving 1 sector was deleted is realistically enough. if i need to uncover tampering in the future i can improve this.
        }
        return state;
    }
}