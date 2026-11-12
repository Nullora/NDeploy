#pragma once
#include"tkn.hpp"
#include"metadata.hpp"
#include<dir.hpp>
#include<handler.hpp>
#include<vector>
#include<string>
#include<iostream>

namespace cmd{
    void handleCommands(CMDS& cmd, const std::vector<std::string>& variables, const std::string& alt_stream, const std::string& rep_path){
        if(cmd==CMDS::UNK) return;
        mdata::FileRep file;
        mdata::RETURN mdata_error;
        dir::RETURN dir_error;
        switch(cmd){
            case CMDS::SIGN:
                //SIGN FILE
                file =  mdata::SignFile(variables[0], variables[1], alt_stream, 1);
                mdata_error = file.last_return;
                if(mdata_error!=mdata::SIGN_SUCCESS){
                    std::cout<<handler::HandleErrorMessage(mdata_error);
                    break;
                }
                //MAKE OBJECT
                dir_error = dir::MakeRepositoryObject(file, rep_path);
                if(dir_error!=dir::SUCCESS){
                    std::cout<<handler::HandleErrorMessage(dir_error);
                    break;
                }
                std::cout<<"Successfully signed file. \n";
                break;
            case CMDS::UNSIGN:
                if(mdata::UnsignFile(variables[0], alt_stream)){
                    std::cout<<"unsignedfile. \n";
                }
                break;
        }
    }
}