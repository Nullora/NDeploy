#pragma once
#include"tkn.hpp"
#include<vector>
#include<string>

namespace cmd{
    void handleCommands(CMDS& cmd, std::vector<std::string>& variables){
        if(cmd==CMDS::UNK) return; //if unknown command
        //will come soon
    }
}