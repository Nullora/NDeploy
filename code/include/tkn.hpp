#pragma once
#include<string>
#include<unordered_map>


enum class CMDS {
    SIGN,
    IMPORT,
    UPDATE,
    PUSH,
    RENAME,
    UNK
};

class tkn{ //tokenize command into enum
public:
    static inline std::unordered_map<std::string, CMDS> command_map;
    static void init_cmdMap(){
        //much faster than previous if/else slop
        command_map["sign"] = CMDS::SIGN;
        command_map["import"] = CMDS::IMPORT;
        command_map["update"] = CMDS::UPDATE;
        command_map["push"] = CMDS::PUSH;
        command_map["rename"] = CMDS::RENAME;
    }

    static CMDS GetCommand(const std::string& input){
        auto it = command_map.find(input);

        if(it != command_map.end()){
            return it->second;
        }
        return CMDS::UNK;
    }
};