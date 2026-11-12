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
    cmd::handleCommands(command, variables, alt_stream, rep_name);
    //[temp]
    vector<filesystem::path> signed_files = dir::GetAllSignedFiles(filesystem::current_path(), alt_stream);
    for(auto& file : signed_files){
        cout << mdata::ReadMData(file.string(), alt_stream, "ID") << endl;
        cout << mdata::ReadMData(file.string(), alt_stream, "CONTENT_HASH") << endl;
    }
    for(auto& [id, hash] : FileHandle::id_hash_map){
        cout<<id<<": "<<hash<<endl;
    }
    FileHandle::save_id_hash(id_hash_path);

}