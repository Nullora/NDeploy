#pragma once
#include"tkn.hpp"
#include"metadata.hpp"
#include<dir.hpp>
#include<handler.hpp>
#include<vector>
#include<string>
#include<iostream>
#include<filesystem>

namespace cmd{
    std::string handleCommands(CMDS& cmd, const std::vector<std::string>& variables, const std::string& alt_stream, const std::string& rep_path){
        if(cmd==CMDS::UNK) return "unknown command";
        mdata::FileRep file;
        mdata::RETURN mdata_error;
        dir::RETURN dir_error;

        std::vector<std::filesystem::path> signed_files;
        std::string ret="";
        switch(cmd){
            case CMDS::SIGN:
                //SIGN FILE
                file =  mdata::SignFile(variables[0], variables[1], alt_stream, 1);
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
                dir_error = dir::ImportRepositoryObject(variables[0], std::filesystem::current_path().string(), variables[1], rep_path, alt_stream);
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
                signed_files = dir::GetAllSignedFiles(std::filesystem::current_path(), alt_stream);
                for(auto& file : signed_files){
                    ret += "Signed Files in CWD >> " + mdata::ReadMData(file.string(), alt_stream, "ID") + ": " + mdata::ReadMData(file.string(), alt_stream, "CONTENT_HASH") + '\n';
                }
                for(auto& [id, hash] : FileHandle::id_hash_map){
                    ret += "Local Hash Index >> " + id + ": " + hash + '\n';
                }
                return ret;
            
            case CMDS::RCALC:
                dir_error = dir::calcCWDHash(alt_stream);
                if(dir_error!=dir::SUCCESS){
                    return handler::HandleErrorMessage(dir_error);
                }
                return "Manually recalculated all hashes in current working directory. `ndep all` to see updated hashes. \n";
            
            case CMDS::RENAME:
                dir_error = dir::renameLibrary(variables[0], alt_stream);
                if(dir_error!=dir::SUCCESS){
                    return handler::HandleErrorMessage(dir_error);
                }
                return "Found. \n";
        }
        return "ERROR?:745 \n";
    }
}