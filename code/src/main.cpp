#include<iostream>
#include"tkn.hpp"
#include"metadata.hpp"
#include"dir.hpp"
#include<vector>
#include <string>
using namespace std;

int main(int argc, char* argv[]){
    if(argc < 2) return 0;

    //initializing
    tkn::init_cmdMap();

    //fill vector with arguments
    vector<string> variables(argv + 2, argv + argc);

    //get command using tkn
    CMDS command = tkn::GetCommand({argv[1]}); //fixed argv[1] to get first argument

    vector<filesystem::path> files = dir::GetAllFiles(filesystem::current_path());
    for(auto& file : files){
        cout << file << endl;
    }

}