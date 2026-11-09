#pragma once
#include<vector>
#include<filesystem>
#include<metadata.hpp>

namespace dir{ //directory search
    std::vector<std::filesystem::path> GetAllFiles(std::filesystem::path path){
        std::vector<std::filesystem::path> all_files;
        if(!std::filesystem::exists(path)) return {};
            
        std::filesystem::recursive_directory_iterator it(path);
        

        std::filesystem::recursive_directory_iterator end; //idk if this is a good way to do it but its all i could think of.
        while(it != end){
            all_files.push_back(it->path());
            ++it;
        }
        return all_files;
        
    }

    std::vector<std::filesystem::path> GetAllSignedFiles(std::filesystem::path path, std::wstring alt_stream){
        std::vector<std::filesystem::path> all_files = GetAllFiles(path);
        std::vector<std::filesystem::path> signed_files = {};
        for(auto& path : all_files){
            if(mdata::ReadId(path, alt_stream)!=L"UNSIGNED"){
                signed_files.push_back(path);
            }
        }
        return signed_files;
    }
}