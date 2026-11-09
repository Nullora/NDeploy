#include<string>
#include<vector>
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
private:
    static std::unordered_map<std::string, CMDS> command_map;
public:

    static void init_cmdMap(){
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