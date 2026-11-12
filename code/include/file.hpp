#pragma once
#include <Windows.h>
#include<unordered_map>
#include<filesystem>
#include<fstream>


class FileHandle{
public:
    static inline std::unordered_map<std::string, std::string> id_hash_map; //id -> hash

    static bool save_id_hash(const std::string& path){
        std::filesystem::create_directories(std::filesystem::path(path).parent_path());
        HANDLE file = CreateFileA(path.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr); 
        if(file==INVALID_HANDLE_VALUE) return false;
        std::string out;
        for(auto& [id, hash] : id_hash_map){
            out += "ihm:" + id + " " + hash + '\n';
        }
        DWORD written;
        bool success = WriteFile(file, out.data(), static_cast<DWORD>(out.size()), &written, nullptr);
        CloseHandle(file);
        return success && written == out.size();
    }

    static void load_id_hash(const std::string& path){
        std::ifstream inW;
        inW.open(path);
        std::string lineW;
        while(std::getline(inW, lineW)){
            if(!lineW.empty()){
                //get id_hash_map data;
                if(lineW.starts_with("ihm:")){
                    lineW = lineW.substr(4);
                    std::string id, hash;
                    std::stringstream ss(lineW);
                    ss >> id >> hash;
                    id_hash_map.emplace(id, hash);
                }
            }
        }
        inW.close();
    }
};