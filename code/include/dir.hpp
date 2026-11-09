#pragma once
#include<vector>
#include<filesystem>

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

    
}