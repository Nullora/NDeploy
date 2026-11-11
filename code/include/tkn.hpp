#pragma once
#include<string>
#include<unordered_map>
#include<statics.hpp>
#include<cmds.hpp>

namespace tkn{
    CMDS GetCommand(const std::string& input){
        auto it = statics::command_map.find(input);

        if(it != statics::command_map.end()){
            return it->second;
        }
        return CMDS::UNK;
    }
};