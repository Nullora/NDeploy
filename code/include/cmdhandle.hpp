#pragma once
#include"tkn.hpp"
#include"metadata.hpp"
#include<handler.hpp>
#include<dir.hpp>

#include<vector>
#include<string>
#include<iostream>
#include<filesystem>

namespace cmd{
    std::string handleCommands(CMDS& cmd, const std::vector<std::string>& variables, std::vector<std::string>& flags, const std::string& alt_stream, const std::string& rep_path){
        if(cmd==CMDS::UNK) return "unknown command";
        mdata::FileRep file;
        mdata::RETURN mdata_error;
        dir::RETURN dir_error;

        std::vector<std::filesystem::path> signed_files;
        std::string ret="";
        switch(cmd){
            case CMDS::SIGN:
                //SIGN FILE
                file =  mdata::SignFile(variables[0], variables[1], alt_stream);
                mdata_error = file.last_return;
                if(mdata_error!=mdata::SIGN_SUCCESS){
                    return handler::HandleErrorMessage(mdata_error);
                }
                //MAKE OBJECT
                dir_error = dir::MakeRepositoryObject(file, rep_path, alt_stream);
                if(dir_error!=dir::SUCCESS){
                    return handler::HandleErrorMessage(dir_error);
                }
                return "Successfully signed file. \n";

            case CMDS::UNSIGN:
                if(mdata::UnsignFile(variables[0], alt_stream)){
                    return "unsigned file. \n";
                }
                return "error unsigning file \n";
            
            case CMDS::IMPORT:
                dir_error = dir::ImportRepositoryObject(variables[0], std::filesystem::current_path().string(), rep_path, alt_stream);
                if(dir_error!=dir::SUCCESS){
                    return handler::HandleErrorMessage(dir_error);
                }
                return "Imported file. \n";
            
            //slop slop slop slop push
            case CMDS::PUSH:
                dir_error = dir::UpdateRepositoryObject(variables[0], variables[1], rep_path, alt_stream);
                if(dir_error!=dir::SUCCESS){
                    return handler::HandleErrorMessage(dir_error);
                }
                return "Pushed update to repo object. \n";
            
            case CMDS::UPDATE:
                dir_error = dir::UpdateImportedFile(variables[0], alt_stream, rep_path);
                if(dir_error!=dir::SUCCESS){
                    return handler::HandleErrorMessage(dir_error);
                }
                return "Updated library successfully. \n";
            
            case CMDS::ALL:
                for(auto& [id, hash] : FileHandle::id_hash_map){
                    ret += "Hash Index >> " + id + ": " + hash + '\n';
                }
                return ret;

            case CMDS::CWD:
                signed_files = dir::GetAllSignedFiles(std::filesystem::current_path(), alt_stream);
                for(auto& file : signed_files){
                    ret += "CWD >> " + mdata::ReadMData(file.string(), alt_stream, "ID") + ": " + mdata::ReadMData(file.string(), alt_stream, "CONTENT_HASH") + '\n';
                }
                return ret;
            
            case CMDS::RCALC:
                dir_error = dir::calcCWDHash(alt_stream);
                if(dir_error!=dir::SUCCESS){
                    return handler::HandleErrorMessage(dir_error);
                }
                return "Manually recalculated all hashes in current working directory. `ndep all` to see updated hashes. \n";
            
            case CMDS::RENAME:
                dir_error = dir::renameLibrary(variables[0],variables[1], alt_stream);
                if(dir_error!=dir::SUCCESS){
                    return handler::HandleErrorMessage(dir_error);
                }
                return "Renamed library successfully. \n";

            case CMDS::FIND:
                return dir::findAllLibraryPaths(variables[0]);

            case CMDS::CWDF:
                signed_files = dir::GetAllSignedFiles(std::filesystem::current_path(), alt_stream);
                for(auto& file : signed_files){
                    if(mdata::ReadMData(file.string(), alt_stream, "ID")==variables[0]){
                        ret += variables[0] + " >> " + file.string() + "\n";
                    }
                }
                return ret;

            case CMDS::ADDTOGROUP:
                dir_error = dir::AddFileToGroup(variables[1], variables[0]);
                if(dir_error!=dir::SUCCESS) return handler::HandleErrorMessage(dir_error);
                return "Added to group. \n";
            
            case CMDS::IMPORTGROUP:
                dir_error = dir::ImportGroup(variables[0], rep_path, alt_stream);
                if(dir_error!=dir::SUCCESS) return handler::HandleErrorMessage(dir_error);
                return "Imported group. \n";

            case CMDS::UPDATEGROUP:
                dir_error = dir::UpdateGroup(variables[0], rep_path, alt_stream);
                if(dir_error!=dir::SUCCESS) return handler::HandleErrorMessage(dir_error);
                return "Updated group. \n";

            case CMDS::ALLG:
                for(auto& [group, it] : FileHandle::group_id_map){
                    ret += "Group Index >> " + group + "\n";
                }
                return ret;
            case CMDS::ALLIG:
                ret += "In group " + variables[0] + ": \n";
                for(auto& [id, placeholder] : FileHandle::group_id_map[variables[0]]){
                    ret += "    Id >> " + id + "\n";
                }
                return ret;

            case CMDS::CONFIG:
                for(auto& flag : flags){
                    if(!flag.starts_with('-')) continue; //gotta check, u never know :P
                    const size_t eq = flag.find('=');
                    if(eq==std::string::npos) continue;
                    const std::string key = flag.substr(1, eq-1);
                    const std::string value = flag.substr(eq+1);
                    auto it = statics::configs_map.find(key);
                    if(it==statics::configs_map.end()) return "Flag not found. \n";
                    FileHandle::configs[it->second] = value;
                }
                return "Completed config. \n";
            
            case CMDS::LISTCONFIG:
                for(auto& [config, value] : FileHandle::configs){
                    const auto it = std::find_if(
                        statics::configs_map.begin(),
                        statics::configs_map.end(),
                        [config](const auto& entry){
                            return entry.second == config;
                        }
                    );
                    if(it != statics::configs_map.end()){
                        ret += "> " + it->first + " = " + value + "\n";
                    }else{
                        ret += "-unknown_config="+value+"\n";
                    }
                }
                return ret;
        }
        return "ERROR?:745 \n"; //random ass error code btw lol. just wanted to see some cool numbers if things go south
    }
}