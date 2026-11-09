#pragma once
#include<vector>
#include<filesystem>
#include<metadata.hpp>


namespace dir{ //finding files, filtering files, and managing original files.
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
}