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
                dir_error = dir::ImportRepositoryObject(variables[0], std::filesystem::current_path().string(), rep_path, alt_stream);
                if(dir_error!=dir::SUCCESS){
                    return handler::HandleErrorMessage(dir_error);
                }
                return "Imported file. \n";
            
            case CMDS::PUSH:
                
        }
        return "ERROR? \n";
    }
}