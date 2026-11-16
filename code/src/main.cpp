#include<tkn.hpp>
#include<metadata.hpp>
#include<dir.hpp>
#include<cmdhandle.hpp>
#include<file.hpp>
#include<vector>
#include <string>
using namespace std;

const string alt_stream = "NDEP";
const string rep_name = "C:\\Users\\mehdi\\Projects\\ndep\\.ndep"; //permanent repository
const string id_hash_path = rep_name + "\\data\\ihp";

int main(int argc, char* argv[]){
    if(argc < 2) return 0;
    FileHandle::load_id_hash(id_hash_path);
    //fill vector with arguments
    vector<string> variables(argv + 2, argv + argc);

    //get command using tkn
    CMDS command = tkn::GetCommand({argv[1]}); //fixed argv[1] to get first argument
    string ret = cmd::handleCommands(command, variables, alt_stream, rep_name);

    //debug with `ndep all` to see all signed files in cwd + all signed files in local hash index

    cout<<ret;
    FileHandle::save_id_hash(id_hash_path);
    //popopopopoppo
    //yooooo
}