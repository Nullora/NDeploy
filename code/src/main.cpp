#include<iostream>
#include"tkn.hpp"
#include"metadata.hpp"
#include"dir.hpp"
#include<vector>
#include <string>
using namespace std;

const string alt_stream = "NDEP_ID";

int main(int argc, char* argv[]){
    if(argc < 2) return 0;

    //initializing
    tkn::init_cmdMap();

    //fill vector with arguments
    vector<string> variables(argv + 2, argv + argc);

    //get command using tkn
    CMDS command = tkn::GetCommand({argv[1]}); //fixed argv[1] to get first argument

    //[temp]
    vector<filesystem::path> signed_files = dir::GetAllSignedFiles(filesystem::current_path(), alt_stream);
    for(auto& file : signed_files){
        cout << file << endl;
        cout << mdata::ReadId(file.string(), alt_stream) << endl;
    }

}