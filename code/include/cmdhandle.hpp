#pragma once
#include"tkn.hpp"
#include"metadata.hpp"
#include<vector>
#include<string>
#include<iostream>

namespace cmd{
    void handleCommands(CMDS& cmd, const std::vector<std::string>& variables, const std::string& alt_stream){
        if(cmd==CMDS::UNK) return;

        switch(cmd){
            case CMDS::SIGN:
                if(mdata::SignFile(variables[0], variables[1], alt_stream, 1).last_return==RETURN::SIGN_SUCCESS){
                    std::cout << "Successfully signed file. \n";
                }
                break;
            case CMDS::UNSIGN:
                if(mdata::UnsignFile(variables[0], alt_stream)){
                    std::cout<<"Successfully unsigned file. \n";
                }
                break;
        }
    }
}