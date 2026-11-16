#pragma once

#include<unordered_map>
#include<string>


#include<dir.hpp>
#include<metadata.hpp>
#include<cmds.hpp>

class statics{
public:
    //top 1 immediately invoked lambda expressions down below;
    //anyone looking for a goated dev hmu :3

    static inline const std::unordered_map<std::string, CMDS> command_map = []{
        std::unordered_map<std::string, CMDS> m;
        m["sign"] = CMDS::SIGN;
        m["unsign"] = CMDS::UNSIGN;
        m["import"] = CMDS::IMPORT;
        m["update"] = CMDS::UPDATE;
        m["push"] = CMDS::PUSH;
        m["rename"] = CMDS::RENAME;
        m["all"] = CMDS::ALL;
        m["rcalc"] = CMDS::RCALC;
        return m;
    }();

    static inline const std::unordered_map<dir::RETURN, std::string> dir_error_map = []{
        std::unordered_map<dir::RETURN, std::string> m;
        m[dir::FAILURE] = "Failed to write to file. \n";
        m[dir::SUCCESS] = "Successfuly made repository object. \n";
        m[dir::INVALID_HASH] = "Recalculated hash doesnt match reported hash. \n";
        m[dir::FILE_EXISTS] = "File already exists. \n";
        m[dir::COMPRESSION_ERROR] = "Compression error. \n";
        m[dir::INVALID_HANDLE] = "Invalid file handle from `CreateFileA` \n";
        m[dir::INVALID_FILE] = "Invalid repository object. \n";
        m[dir::INVALID_NAME] = "Library name doesnt exist.\n";
        m[dir::UPTODATE] = "Library is already up to date. \n";
        m[dir::UNSIGNED] = "File is unsigned. \n";
        m[dir::NAME_MATCH_ERROR] = "Library name doesnt match source file name. \n";
        return m;
    }();

    static inline const std::unordered_map<mdata::RETURN, std::string> mdata_error_map = []{
        std::unordered_map<mdata::RETURN, std::string> m;
        m[mdata::INVALID_NAME] = "Invalid library name. Either already used or contains character `:` \n";
        m[mdata::INVALID_FILE] = "Invalid file. \n";
        m[mdata::SIGN_SUCCESS] = "Successfully signed file. \n";
        m[mdata::FILE_ALREADY_SIGNED] = "File already has a signature. Use force_sign if you wanna overwrite it. \n";
        m[mdata::FAIL] = "Failed to update file metadata. \n";
        return m;
    }();
};
