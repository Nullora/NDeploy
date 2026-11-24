#include<tkn.hpp>
#include<metadata.hpp>
#include<cmdhandle.hpp>
#include<file.hpp>
#include<vector>
#include <string>
using namespace std;

const string alt_stream = "NDEP";

const string rep_name = "C:\\NFramework\\.ndep"; //permanent repository, will change to a dynamic repo so you can switch repos and segment library genres into their own repo.
const string id_hash_path = rep_name + "\\data\\ihp";//id_hash_path
const string id_fileid_path = rep_name + "\\data\\ifp";//id_fileid_path
const string group_ids_path = rep_name + "\\data\\gip";//group_id_path
const string config_path = rep_name + "\\config\\config";//config
const string last_push_path = rep_name + "\\config\\lpp";//last_push_path

int main(int argc, char* argv[]){
    if(argc < 2) return 0;

    FileHandle::load_id_hash(id_hash_path);
    FileHandle::load_id_fileid(id_fileid_path);
    FileHandle::load_group_id(group_ids_path);
    FileHandle::load_config(config_path);
    //refresh id_fileid
    mdata::RefreshID_FILEID_map();

    //get variables
    vector<string> variables(argv + 2, argv + argc);

    //get flags (not in use yet.)
    vector<string> flags= {};
    for(auto it = variables.begin(); it != variables.end();){
        if(it->starts_with("-")){
            flags.push_back(*it);
            it = variables.erase(it);
        }else{
            ++it;
        }
    }


    //> `ndep push {libname} -last` uses the last source of push instead of rewriting `ndep push {libname} {source_of_push}` everytime
    //> `ndep update {libname} -v:1.2.0` updates the library to a specific version instead of last version
    //> WARNING: keep in mind these arent implemented yet, only ideas for the future

    
    CMDS command = tkn::GetCommand({argv[1]});
    string ret = cmd::handleCommands(command, variables, flags, alt_stream, rep_name);


    cout<<ret;
    FileHandle::save_id_hash(id_hash_path);
    FileHandle::save_id_fileid(id_fileid_path);
    FileHandle::save_group_id(group_ids_path);
    FileHandle::save_config(config_path);

}