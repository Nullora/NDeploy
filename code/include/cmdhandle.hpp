#pragma once
#include"tokenize.hpp"
#include<vector>
#include<string>

namespace cmd{
    void handleCommands(tkn::CMDS& cmd, std::vector<std::string>& variables){
        if(cmd==tkn::UNK) return; //if unknown command
        //will come soon
    }
}