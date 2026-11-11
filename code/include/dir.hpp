#pragma once
#include<vector>
#include<filesystem>
#include<metadata.hpp>
#include<hash.hpp>
#include"compression.hpp"
#include<statics.hpp>
#include<iostream>



namespace dir{ //finding files, filtering files, and managing original files.

    enum RETURN{
        INVALID_HASH,
        FILE_EXISTS,
        COMPRESSION_ERROR,
        SUCCESS,
        INVALID_HANDLE,
        FAILURE
    };

    std::vector<std::filesystem::path> GetAllFiles(std::filesystem::path path){
        std::vector<std::filesystem::path> all_files;
        if(!std::filesystem::exists(path)) return {};
            
        std::filesystem::recursive_directory_iterator it(path);
        

        std::filesystem::recursive_directory_iterator end; //idk if this is a good way to do it but its all i could think of :c
        while(it != end){
            all_files.push_back(it->path());
            ++it;
        }
        return all_files;
        
    }

    std::vector<std::filesystem::path> GetAllSignedFiles(std::filesystem::path path, std::string alt_stream){
        std::vector<std::filesystem::path> all_files = GetAllFiles(path);
        std::vector<std::filesystem::path> signed_files = {};
        for(auto& path : all_files){
            if(mdata::ReadMData(path.string(), alt_stream, "ID")!="UNSIGNED"){
                signed_files.push_back(path);
            }
        }
        return signed_files;
    }


    RETURN MakeRepositoryObject(mdata::FileRep& filerep, std::string rep_path){
        std::ifstream file0(filerep.og_path, std::ios::binary);
        //c++ interepts it as a function so we use double parentheses to clarify declaration. we could use brace initialization but ->
        //that can select `intialized_list` ->
        //constructor if possible, to focrce iterator range constructor we instead use double parentheses :3
        std::vector<unsigned char> bytes ( (std::istreambuf_iterator<char>(file0) ), std::istreambuf_iterator<char>() );
        file0.close();
        std::string content_hash = nhash::SHA256File(filerep.og_path);

        //debug
        std::cout << "Hash: [" << content_hash << "]\n";
        std::cout << "Stored hash: [" << filerep.content_hash << "]\n";
        std::cout << "Hash length: " << content_hash.size() << '\n';

        //keep going |
        if(content_hash != filerep.content_hash) return INVALID_HASH;
        //compress
        std::vector<unsigned char> compressed_bytes;
        if(!compression::Compress(bytes, compressed_bytes)) return COMPRESSION_ERROR;
        //construct object path.
        std::string name = filerep.content_hash.substr(2); //filename is remainder of content hash
        std::string dir = filerep.content_hash.substr(0,2); //parent folder of filename is first 2 characters of content hash
        std::filesystem::path object_path = std::filesystem::path(rep_path) / "objects" / dir / name;
        if (std::filesystem::exists(object_path)) return FILE_EXISTS;

        std::filesystem::create_directories(object_path.parent_path()); //create parent folders if they dont exist
        HANDLE file = CreateFileA(object_path.string().c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
        if(file==INVALID_HANDLE_VALUE) return INVALID_HANDLE;

        DWORD written;
        bool write = WriteFile(file, compressed_bytes.data(), static_cast<DWORD>(compressed_bytes.size()), &written, nullptr);
        CloseHandle(file);
        if(write && written == compressed_bytes.size()) return SUCCESS;
        return FAILURE;
    }
}