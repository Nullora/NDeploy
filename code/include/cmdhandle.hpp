#pragma once
#include"tkn.hpp"
#include"metadata.hpp"
#include<dir.hpp>
#include<vector>
#include<string>
#include<iostream>

namespace cmd{
    void handleCommands(CMDS& cmd, const std::vector<std::string>& variables, const std::string& alt_stream, const std::string& rep_name){
        if(cmd==CMDS::UNK) return;
        FileRep file;
        switch(cmd){
            case CMDS::SIGN:
                file = mdata::SignFile(variables[0], variables[1], alt_stream, 1);
                if(file.last_return!=RETURN::SIGN_SUCCESS) break;
                if(!dir::MakeRepositoryObject(file, rep_name)) break;
                std::cout<<"Successfully signed file. \n";
                break;
            case CMDS::UNSIGN:
                if(mdata::UnsignFile(variables[0], alt_stream)){
                    std::cout<<"Successfully unsigned file. \n";
                }else{
                    std::cout<<"Error unsigning file \n";
                }
                break;
        }
    }
}