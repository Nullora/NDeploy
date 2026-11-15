#pragma once
#include <Windows.h>
#include <string>
#include<filesystem>
#include<hash.hpp>
#include<statics.hpp>
#include<file.hpp>

#include <sys/stat.h>
#include <fstream>

#include<iostream>



namespace mdata{ //metadata handling


    enum RETURN {
        FILE_ALREADY_SIGNED,
        SIGN_SUCCESS,
        INVALID_FILE,
        INVALID_NAME,
        FAIL
    };

    struct FileRep{
        RETURN last_return;
        std::string id;
        std::string content_hash;
        std::string copy_id;
        std::string version_id;
        std::filesystem::path og_path;
    };

    inline bool exists(const std::string& name) {
        struct stat buffer;   
        return (stat (name.c_str(), &buffer) == 0); 
    }

    bool WriteMData(const std::string& filename, const std::string& alt_stream, const std::string& stream_sector,const std::string& data){
        std::string stream = filename + ":" + alt_stream + "_" + stream_sector;
        if(!exists(filename)) return false;
        HANDLE file = CreateFileA(stream.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr); 
        if(file==INVALID_HANDLE_VALUE) return false;
        DWORD written;
        bool success = WriteFile(file, data.data(), static_cast<DWORD>(data.size()), &written, nullptr);
        CloseHandle(file);
        return success && written == data.size();
    }

    std::string ReadMData(const std::string& filename, const std::string& alt_stream, const std::string& stream_sector){ //ID, CONTENT_HASH, COPY_ID, VERSION_ID
        std::string stream = filename + ":" + alt_stream + "_" + stream_sector;

        HANDLE file = CreateFileA(stream.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
        if(file==INVALID_HANDLE_VALUE) 	return "UNSIGNED"; //ALTERNATE STREAM DOESNT EXIST

        char buffer[100] = {}; //for now can only up to 50 chars, if any more than that were fucked. ill fix later
        DWORD bytes_read;
        bool success = ReadFile(file, buffer, sizeof(buffer), &bytes_read, nullptr);
        CloseHandle(file);
        if(!success) return "UNSIGNED"; // ERROR READING ALTERNATE STREAM
        return std::string(buffer,bytes_read);

    }
    
    //ai function, im not gonna write this slop it already uses functions i wrote so technically i wrote it
    bool ApplyHeaderData(
        const std::filesystem::path& path,
        const std::string& alt_stream,
        const std::string& name = "",
        const std::string& content_hash = "",
        const std::string& copy_id = "",
        const std::string& version_id = ""
    ){
        if(!name.empty())
            if(!WriteMData(path.string(), alt_stream, "ID", name)) return false;

        if(!content_hash.empty())
            if(!WriteMData(path.string(), alt_stream, "CONTENT_HASH", content_hash)) return false;

        if(!copy_id.empty())
            if(!WriteMData(path.string(), alt_stream, "COPY_ID", copy_id)) return false;

        if(!version_id.empty())
            if(!WriteMData(path.string(), alt_stream, "VERSION_ID", version_id)) return false;

        return true;
    }

    FileRep SignFile(std::filesystem::path path, std::string name, std::string alt_stream, bool force_sign = false){
        FileRep lib;
        lib.last_return = RETURN::FAIL;
        //check if name is taken. we do this at the top so we dont waste computing power before checking
        auto it = FileHandle::id_hash_map.find(name);
        if(it!=FileHandle::id_hash_map.end() && !force_sign){
            lib.last_return = RETURN::INVALID_NAME;
            return lib;
        }


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

        //write content hash. used to compare 2 files of the same identity but imported in different projects. useful when `ndep update` is used to not cause conflict between both versions
        std::ifstream file(path, std::ios::binary);
        //most vexing parse slop
        std::vector<unsigned char> bytes((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        std::string content_hash = nhash::SHA256File(path);

        bool succ = ApplyHeaderData(path, alt_stream, name, content_hash, "OG_LIB", "1.0.0");

        lib.id = name;
        lib.content_hash = content_hash;
        lib.copy_id = "OG_LIB";
        lib.version_id = "1.0.0";
        lib.og_path = path;
        if(succ) lib.last_return = RETURN::SIGN_SUCCESS;

        std::cout<<"SIGNFILE() >> "<<name<<": "<<content_hash<<std::endl;
        FileHandle::id_hash_map[name] = content_hash;

        return lib;
    }

    bool UnsignFile(const std::string path, const std::string& alt_stream){
        std::vector<std::string> stream_sectors = {"ID", "CONTENT_HASH", "COPY_ID", "VERSION_ID"};
        //check existence
        std::string name = ReadMData(path, alt_stream, "ID");
        if(name=="UNSIGNED")return false;

        std::string stream;
        bool state = true;
        for (auto& sector : stream_sectors) {
            stream = path + ":" + alt_stream + "_" + sector;
            if (!DeleteFileA(stream.c_str())) state = false;
        }

        if(state) FileHandle::id_hash_map.erase(name);
        return state;
    }
}