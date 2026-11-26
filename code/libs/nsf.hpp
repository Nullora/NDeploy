#pragma once
#include <filesystem>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <iostream>

#define WIN32_LEAN_AND_MEAN 
#include<Windows.h>

#include<hash.hpp>


namespace nsf{
    enum LAST_E{
        //OpenFile()
        OPEN_FAIL,
        OPEN_SUCCESS,
    };
    //file handle. fancy slop i stole from windows sdk
    typedef struct NSF_FILE{
        std::string value;
        bool valid;
    }NSF_FILE;

    typedef struct NSF_HEADER{
        bool valid;
        std::unordered_map<std::string, std::string> contents;
    }NSF_HEADER;


    inline std::unordered_map<std::string, std::string> filebytes {};
    //Open file and return handle.
    static NSF_FILE OpenFile(std::filesystem::path path){
        //open the file
        std::ifstream file0(path, std::ios::binary);
        if (!file0.is_open()){
            #ifdef NSF_DEBUG
                std::cout<<"NSF DEBUG [OpenFile] > Could not open file `"<<path.string()<<"`. \n";
            #endif
             return {"", false};
        }
        //read
        std::string hash = nhash::SHA256File(path);
        filebytes[hash] = std::string(
            std::istreambuf_iterator<char>(file0), std::istreambuf_iterator<char>()
        );
        //return
        file0.close();

        //debug
        #ifdef NSF_DEBUG
            std::cout<<"NSF DEBUG [OpenFile] > "<<path.string()<<" = "<<hash<<std::endl;
        #endif

        return {hash, true};
    }

    //Close file and return {1, 0}
    static bool CloseFile(const NSF_FILE& handle){
        if(!handle.valid){
            #ifdef NSF_DEBUG
                std::cout<<"NSF DEBUG [CloseFile] > Invalid Handle passed. \n";
            #endif
             return false;
        }
             auto it = filebytes.find(handle.value);
        if(it==filebytes.end()) return false;
        
        filebytes.erase(it);
        return true;
    }

    //Get header contents. returns NSF_HEADER with map: config -> value.
    static NSF_HEADER GetHeaderContents(const NSF_FILE& handle, const std::string& header){
        if(!handle.valid) return {false};
        std::istringstream stream(filebytes[handle.value]);
        std::string line;
        bool in_header = false;
        NSF_HEADER return_header = {true, {}};
        while(std::getline(stream, line)){
            //check for header start. `[/header_name]`
            if(line.starts_with("[/")){
                line = line.substr(2);
                const size_t end = line.find(']'); if(end == std::string::npos) continue;
                const std::string header_str = line.substr(0, end);
                //found header.
                if(header == header_str){
                    in_header = true;
                    #ifdef NSF_DEBUG
                        std::cout<<"NSF DEBUG [GetHeaderContents] > Started header `"<<header_str<<"`. \n";
                    #endif
                    continue;
                }
            }
            //check for header end. `[\header_name]`
            if(line.starts_with("[\\")){
                line = line.substr(2);
                const size_t end = line.find(']'); if(end ==std::string::npos) continue;
                const std::string header_str = line.substr(0, end);
                //end of header
                if(header==header_str){
                    in_header = false;
                    #ifdef NSF_DEBUG
                        std::cout<<"NSF DEBUG [GetHeaderContents] > Ended header `"<<header_str<<"`. \n";
                    #endif
                    continue;
                }
            }
            //get contents of header.
            //[/header1]
            //config1=hello
            //config2=world
            //[\header1]
            if(!line.empty() && in_header){
                const size_t sep = line.find('=');
                if(sep==std::string::npos) continue;
                const std::string config = line.substr(0,sep);
                const std::string value = line.substr(sep+1);
                return_header.contents[config] = value;
            }
        }
        return return_header;
    }
}