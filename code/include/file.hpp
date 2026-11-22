#pragma once
#include <Windows.h>
#include<unordered_map>
#include<filesystem>
#include<fstream>
#include<vector>
#include <sstream>
#include <iomanip>
#include <string>

    typedef struct FileInformation{
        ULONGLONG VolumeSN;
        FILE_ID_128 FileId;
        bool lasterror;
    }FileInformation;

class FileHandle{
public:
    static inline std::unordered_map<std::string, std::string> id_hash_map; //id -> hash
    static inline std::unordered_map<std::string, std::vector<FileInformation>> id_fileid_map; //ndep id -> windows file id. used to track files globally and find their locations.

    //greatest ai function of all time, im not learning how windows stores its sloppy file ids or how to decode them from hex to a 16byte fileid, go fuck yourself you son of a bitch
    static bool ParseFileID(
        const std::string& hex,
        FILE_ID_128& fileid
    ) {
        if (hex.size() != 32) return false;

        auto hexValue = [](char c) -> int {
            if (c >= '0' && c <= '9') return c - '0';
            if (c >= 'a' && c <= 'f') return c - 'a' + 10;
            if (c >= 'A' && c <= 'F') return c - 'A' + 10;
            return -1;
        };

        for (int i = 0; i < 16; i++) {
            int high = hexValue(hex[i * 2]);
            int low = hexValue(hex[i * 2 + 1]);

            if (high == -1 || low == -1) return false;

            fileid.Identifier[i] = (high << 4) | low;
        }

        return true;
    }

    //functions written by me :3 (not hex slop, im actually so fucking mad at hex rn u legit dont even know thats ong)
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

    static bool save_id_fileid(const std::string& path){
        std::filesystem::create_directories(std::filesystem::path(path).parent_path());
        HANDLE file = CreateFileA(path.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr); 
        if(file==INVALID_HANDLE_VALUE) return false;

        std::string out;
        //construct data to write
        for(auto& [ndepid, fileinfo] : id_fileid_map){
            out += "ifm:" + ndepid + " ";
            for (size_t i = 0; i < fileinfo.size(); i++) {
                auto& entry = fileinfo[i];
                //convert to hex slop
                for (BYTE b : entry.FileId.Identifier) {
                    char buffer[3];
                    snprintf(buffer, sizeof(buffer), "%02X", b);
                    out += buffer;
                }
                //add volume serial number
                out += "|" + std::to_string(entry.VolumeSN);
                if (i + 1 < fileinfo.size()) out += ',';
            }
            out += '\n';
        }
        //write data
        DWORD written;
        bool success = WriteFile(file, out.data(), static_cast<DWORD>(out.size()), &written, nullptr);
        CloseHandle(file);
        return success && written == out.size(); 
    }

    static void load_id_fileid(const std::string& path){
        std::ifstream in;
        in.open(path);
        std::string line;
        while(std::getline(in, line)){
            if(!line.empty()){
                if(line.starts_with("ifm:")){
                    line = line.substr(4);
                    std::string id, fileids_str;
                    std::stringstream ss(line);
                    //populate stringss
                    ss >> id >> fileids_str;

                    if (id.empty() || fileids_str.empty()) continue;
                    std::vector<FileInformation> fileids;

                    std::stringstream entries(fileids_str);
                    std::string entry;

                    while(std::getline(entries, entry, ',')){
                        std::stringstream pair(entry);
                        std::string fileid_hex; //were gonna use parsefileid to get 16byte id 
                        std::string volume_str;
                        if(!std::getline(pair, fileid_hex, '|')) continue;
                        if(!std::getline(pair, volume_str)) continue;

                        FileInformation info{};
                        if(!ParseFileID(fileid_hex, info.FileId)) continue;

                        info.VolumeSN = std::stoull(volume_str);
                        info.lasterror = true;
                        fileids.push_back(info);
                    }
                    id_fileid_map[id] = std::move(fileids); //move instead of copy cz were cool like that (stackoverflow told me to do this)
                }
            }
        }
    }
};