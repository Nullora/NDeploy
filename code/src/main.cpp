#include"tkn.hpp"
#include"metadata.hpp"
#include"dir.hpp"
#include"cmdhandle.hpp"
#include<vector>
#include <string>
using namespace std;

const string alt_stream = "NDEP";
const string rep_name = ".ndep";

int main(int argc, char* argv[]){
    if(argc < 2) return 0;

    //fill vector with arguments
    vector<string> variables(argv + 2, argv + argc);

    //get command using tkn
    CMDS command = tkn::GetCommand({argv[1]}); //fixed argv[1] to get first argument
    cmd::handleCommands(command, variables, alt_stream, rep_name);
    //[temp]
    vector<filesystem::path> signed_files = dir::GetAllSignedFiles(filesystem::current_path(), alt_stream);
    for(auto& file : signed_files){
        cout << file << endl;
        cout << mdata::ReadMData(file.string(), alt_stream, "ID") << endl;
    }

}