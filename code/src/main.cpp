#include<tkn.hpp>
#include<metadata.hpp>
#include<dir.hpp>
#include<cmdhandle.hpp>
#include<file.hpp>
#include<vector>
#include <string>
using namespace std;

const string alt_stream = "NDEP";

const string rep_name = "C:\\NFramework\\.ndep"; //permanent repository, will change to a dynamic repo so you can switch repos and segment library genres into their own repo.
const string id_hash_path = rep_name + "\\data\\ihp";
const string id_fileid_path = rep_name + "\\data\\ifp";

int main(int argc, char* argv[]){
    if(argc < 2) return 0;
    FileHandle::load_id_hash(id_hash_path);
    FileHandle::load_id_fileid(id_fileid_path);
    vector<string> variables(argv + 2, argv + argc);


    //i will add another vector for flags cz some commands will need flags in the future:
    //> `ndep push {libname} -last` uses the last source of push instead of rewriting `ndep push {libname} {source_of_push}` everytime
    //> `ndep update {libname} -v:1.2.0` updates the library to a specific version instead of last version
    //> `ndep sign {filename} {libname} -fs` force sign, overwrites already signed files with new data.
    //> WARNING: keep in mind these arent implemented yet, only ideas for the future, will probably come around next week.

    
    CMDS command = tkn::GetCommand({argv[1]});
    string ret = cmd::handleCommands(command, variables, alt_stream, rep_name);


    cout<<ret;
    FileHandle::save_id_hash(id_hash_path);
    FileHandle::save_id_fileid(id_fileid_path);
}