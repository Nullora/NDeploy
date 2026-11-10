#pragma once
#include<string>
#include<unordered_map>

enum class CMDS {
    SIGN,
    UNSIGN,
    IMPORT,
    UPDATE,
    PUSH,
    RENAME,
    UNK
};

class tkn{
public:
//get rid of a dedicated init() function and just do this
    static inline const std::unordered_map<std::string, CMDS> command_map = []{
        //faster than if/else i had on the linux version
        std::unordered_map<std::string, CMDS> m;
        m["sign"] = CMDS::SIGN;
        m["unsign"] = CMDS::UNSIGN;
        m["import"] = CMDS::IMPORT;
        m["update"] = CMDS::UPDATE;
        m["push"] = CMDS::PUSH;
        m["rename"] = CMDS::RENAME;
        return m;
    }();

    static CMDS GetCommand(const std::string& input){
        auto it = command_map.find(input);

        if(it != command_map.end()){
            return it->second;
        }
        return CMDS::UNK;
    }
};