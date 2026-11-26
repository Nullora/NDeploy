#pragma once
#include <Windows.h>
#include<unordered_map>
#include<filesystem>
#include<fstream>
#include<vector>
#include <sstream>
#include <iomanip>
#include <string>

#include <pugixml.hpp>

typedef struct FileInformation{
    ULONGLONG VolumeSN;
    FILE_ID_128 FileId;
    bool lasterror;
}FileInformation;

enum class CONFIGS{
    //dir
    VS_SOLUTION,
        //debug
    DIR_DEBUG,

    //mdata
    FORCE_SIGN,
        //debug
    MDATA_DEBUG
};

class FileHandle{
public:
    //rep\data\.
    static inline std::unordered_map<std::string, std::string> id_hash_map; //id -> hash
    static inline std::unordered_map<std::string, std::vector<FileInformation>> id_fileid_map; //ndep id -> windows file id. used to track files globally and find their locations.
    static inline std::unordered_map<std::string, std::unordered_map<std::string, int>> group_id_map; //gim[group][id] = placeholder.

    //rep\config\.
    static inline std::unordered_map<CONFIGS, std::string> configs;//configs[config] = value;


    //greatest ai function of all time, im not learning how windows stores its sloppy file ids or how to decode them from hex to a 16byte fileid, go fuck yourself you son of a bitch
    static bool ParseFileID(const std::string& hex, FILE_ID_128& fileid) {
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

    //functions written by me :3 (not hex slop)
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

    static bool save_group_id(const std::string& path){
        bool first = true;
        std::filesystem::create_directories(std::filesystem::path(path).parent_path());
        HANDLE file = CreateFileA(path.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr); 
        if(file==INVALID_HANDLE_VALUE) return false;
        std::string out;
        for(auto& [group, it] : group_id_map){
            out += "\ngim:" + group + " ";
            for(auto& [id, placeholder] : it){
                if(first) {out += id; first=false;}
                else{out += ',' + id;}
            }
        }
        DWORD written;
        bool success = WriteFile(file, out.data(), static_cast<DWORD>(out.size()), &written, nullptr);
        CloseHandle(file);
        return success && written == out.size();
    }

    static void load_group_id(const std::string& path){
        std::ifstream in(path);
        std::string line;

        while(std::getline(in, line)){
            if(!line.starts_with("gim:")) continue;

            line = line.substr(4);
            const size_t separator = line.find(' ');
            if(separator == std::string::npos) continue;

            const std::string group = line.substr(0, separator);
            std::stringstream ids(line.substr(separator + 1));
            std::string id;

            while(std::getline(ids, id, ',')){
                if(!id.empty()) group_id_map[group][id] = 1;
            }
        }
    }

    static bool save_config(const std::string& path){
        std::filesystem::create_directories(std::filesystem::path(path).parent_path());
        HANDLE file = CreateFileA(path.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr); 
        if(file==INVALID_HANDLE_VALUE) return false;
        std::string out;
        
        for(auto& [config, value] : configs){
            out += std::to_string(static_cast<int>(config));
            out += ": ";
            out += value;
            out += '\n';
        }
        DWORD written;
        bool success = WriteFile(file, out.data(), static_cast<DWORD>(out.size()), &written, nullptr);
        CloseHandle(file);
        return success && written == out.size();
    }

    static void load_config(const std::string& path){
        std::ifstream in(path);
        std::string line;
        while(std::getline(in, line)){
            const size_t separator = line.find(':');
            if(separator == std::string::npos) continue;

            std::string config_text = line.substr(0, separator);
            const std::string value = line.substr(separator + 1);

            try{
                int config_number = std::stoi(config_text);
                const CONFIGS config = static_cast<CONFIGS>(config_number);
                std::string trimmed_value = value;

                while(!trimmed_value.empty() && std::isspace(static_cast<unsigned char>(trimmed_value.front()))){
                    trimmed_value.erase(trimmed_value.begin());
                }

                configs[config] = trimmed_value;
            }
            catch(const std::exception&){
                continue;
            }
        }
    }


    static std::vector<std::filesystem::path> GetAllFiles(std::filesystem::path path){
        std::vector<std::filesystem::path> all_files;
        if(!std::filesystem::exists(path)) return {};
            
        std::filesystem::recursive_directory_iterator it(path);
        

        std::filesystem::recursive_directory_iterator end;
        while(it != end){
            all_files.push_back(it->path());
            ++it;
        }
        return all_files;
        
    }

    //ai function cz idk how work pugixml and i dont wanna learn that bs ngl.
    static bool modify_vcxproj(const std::string& cwd, std::string& filename) {
        std::vector<std::filesystem::path> all_files = GetAllFiles(cwd);

        std::filesystem::path vcxproj_file;

        for (auto& file : all_files) {
            if (file.extension() == ".vcxproj") {
                vcxproj_file = file;
                break;
            }
        }

        if (vcxproj_file.empty()) {
            std::cerr << "could not find .vcxproj file\n";
            return false;
        }

        // load xml
        pugi::xml_document doc;

        pugi::xml_parse_result result = doc.load_file(vcxproj_file.string().c_str());

        if (!result) {
            std::cerr << "XML failed: " << result.description() << '\n';
            return false;
        }

        pugi::xml_node root = doc.document_element();

        // file type, rn only source + header.
        std::filesystem::path file_path(filename);
        std::string extension = file_path.extension().string();

        std::string node_type;

        if (extension == ".cpp" || extension == ".c") {
            node_type = "ClCompile";
        }
        else if (extension == ".h" || extension == ".hpp") {
            node_type = "ClInclude";
        }
        else {
            std::cerr << "Unsupported file extension: " << extension << '\n';
            return false;
        }

        // Get path relative to the project file
        std::filesystem::path relative_path =
            std::filesystem::relative(
                std::filesystem::absolute(filename),
                vcxproj_file.parent_path()
            );

        std::string relative_filename = relative_path.generic_string();

        // Find an existing ItemGroup containing the appropriate node
        pugi::xml_node target_group;

        for (pugi::xml_node group : root.children()) {
            if (std::string(group.name()) != "ItemGroup")
                continue;

            for (pugi::xml_node child : group.children()) {
                if (std::string(child.name()) == node_type) {
                    target_group = group;
                    break;
                }
            }

            if (target_group)
                break;
        }

        // Create ItemGroup if one doesn't exist
        if (!target_group) {
            target_group = root.append_child("ItemGroup");
        }

        // Check for duplicates
        for (pugi::xml_node group : root.children()) {
            if (std::string(group.name()) != "ItemGroup")
                continue;

            for (pugi::xml_node child : group.children()) {
                if (std::string(child.name()) == node_type) {
                    if (relative_filename == child.attribute("Include").as_string()) {
                        std::cerr << "File already exists in project\n";
                        return false;
                    }
                }
            }
        }

        // Add file
        pugi::xml_node new_file = target_group.append_child(node_type.c_str());

        new_file.append_attribute("Include") = relative_filename;

        // Save project
        if (!doc.save_file(vcxproj_file.string().c_str(), "    ")) {
            std::cerr << "Failed to save project file\n";
            return false;
        }

        std::cout << "Successfully added " << relative_filename << " to .vcxproj\n";

        return true;
    }
};
