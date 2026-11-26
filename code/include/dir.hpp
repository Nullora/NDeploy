#pragma once
#include<vector>
#include<iostream>
#include<filesystem>
#include<array>
#include<Windows.h>
#include <algorithm>
#ifndef FileIdInfo
#define FileIdInfo ((FILE_INFO_BY_HANDLE_CLASS)18)
#endif
//custom libs
#include<metadata.hpp>
#include<hash.hpp>
#include<compression.hpp>
#include<statics.hpp>
#include<file.hpp>



namespace dir{ //finding files, filtering files, and managing original files and repo objects

//                          ########### UTILITY FUNCTIONS, BARELY USED OUTSIDE OF THIS FILE (EMPHASIS ON BARELY LOL)
    enum RETURN{
        INVALID_FILE,
        INVALID_NAME,
        INVALID_HASH,
        FILE_EXISTS,
        COMPRESSION_ERROR,
        SUCCESS,
        INVALID_HANDLE,
        FAILURE,
        //update
        UPTODATE,
        UNSIGNED,
        NAME_MATCH_ERROR,
        //group
        NEW_GROUP

    };


    std::vector<std::filesystem::path> GetAllFiles(std::filesystem::path path){
        std::vector<std::filesystem::path> all_files;
        if(!std::filesystem::exists(path)) return {};
            
        std::filesystem::recursive_directory_iterator it(path);
        

        std::filesystem::recursive_directory_iterator end; //idk if this is a good way to do it but its all i could think of :c
        while(it != end){
            all_files.push_back(it->path());
            ++it;
        }
        return all_files;
        
    }

    std::vector<std::filesystem::path> GetAllSignedFiles(std::filesystem::path path, std::string alt_stream){
        std::vector<std::filesystem::path> all_files = GetAllFiles(path);
        std::vector<std::filesystem::path> signed_files = {};
        for(auto& path : all_files){
            if(mdata::ReadMData(path.string(), alt_stream, "ID")!="UNSIGNED"){
                signed_files.push_back(path);
            }
        }
        return signed_files;
    }

    RETURN calcCWDHash(const std::string& alt_stream){
        std::vector<std::filesystem::path> signed_files = GetAllSignedFiles(std::filesystem::current_path(), alt_stream);
        std::vector<std::filesystem::path> failed_files;
        std::string calculated_hash;
        for(auto& file : signed_files){
            calculated_hash = "UNSIGNED";
            if(mdata::ReadMData(file.string(),alt_stream,"CONTENT_HASH")!="UNSIGNED"){
                if(!mdata::WriteMData(file.string(), alt_stream, "CONTENT_HASH", nhash::SHA256File(file.string()))){
                    failed_files.push_back(file);
                }
            }
        }
        for(auto& file : failed_files){
            std::cout<<file.string()<<'\n';
        }
        return SUCCESS;
    }




    //                                          ############ NAMESPACE OFFICIAL FUNCTIONS, OUTSIDE FILES RELY ON THESE WORKING
    //################ ndep sign {vars}
    RETURN MakeRepositoryObject(const mdata::FileRep& filerep, const std::string rep_path, const std::string alt_stream){
        std::ifstream file0(filerep.og_path, std::ios::binary);
        std::vector<unsigned char> bytes ( (std::istreambuf_iterator<char>(file0) ), std::istreambuf_iterator<char>() );
        file0.close();
        std::string content_hash = nhash::SHA256File(filerep.og_path);

        //debug
        if(FileHandle::configs[CONFIGS::DIR_DEBUG]=="1"){
            std::cout << "recalculated hash: [" << content_hash << "]\n";
            std::cout << "stored hash: [" << filerep.content_hash << "]\n";
            std::cout << "hash length: " << content_hash.size() << '\n';
        }
        

        //verify calculated hash and reported hash. could have been tampered with, u never know :3
        if(content_hash != filerep.content_hash) return INVALID_HASH;
        //compress
        std::vector<unsigned char> compressed_bytes;
        if(!compression::Compress(bytes, compressed_bytes)) return COMPRESSION_ERROR;

        //construct object path
        std::string name = filerep.content_hash.substr(2); //filename is remainder of content hash
        std::string dir = filerep.content_hash.substr(0,2); //parent folder of filename is first 2 characters of content hash
        std::filesystem::path object_path = std::filesystem::path(rep_path) / "objects" / dir / name;
        if (std::filesystem::exists(object_path)) return FILE_EXISTS;

        std::filesystem::create_directories(object_path.parent_path()); //create parent folders if they dont exist
        HANDLE file = CreateFileA(object_path.string().c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
        if(file==INVALID_HANDLE_VALUE) return INVALID_HANDLE;

        //original size -> metadata of repo object for decompression of object later
        mdata::WriteMData(object_path.string(), alt_stream, "OG_SIZE", std::to_string(bytes.size()));
        mdata::WriteMData(object_path.string(), alt_stream, "OG_FILENAME", filerep.og_path.filename().string()); 
        //apply name + version_id to repository object's alternate stream
        bool succ = mdata::ApplyHeaderData(object_path.string(),alt_stream,filerep.id,"","REP_LIB",filerep.version_id);

        DWORD written;
        bool write = WriteFile(file, compressed_bytes.data(), static_cast<DWORD>(compressed_bytes.size()), &written, nullptr);
        CloseHandle(file);
        if(write && written == compressed_bytes.size() && succ) return SUCCESS;
        return FAILURE;
    }


    // ######## ndep import {lib}
    RETURN ImportRepositoryObject(const std::string& id, std::string target,const std::string& rep_path, const std::string& alt_stream){
        //get hash from id_hash_map
        auto it = FileHandle::id_hash_map.find(id);
        if(it==FileHandle::id_hash_map.end()) return INVALID_NAME;
        std::string hash = it->second;

        //find path for hash
        std::string name = hash.substr(2);
        std::string dir = hash.substr(0,2);
        std::filesystem::path object_path = std::filesystem::path(rep_path) / "objects" / dir / name;
        std::string filename = mdata::ReadMData(object_path.string(), alt_stream, "OG_FILENAME");

        //DIR_DEBUG
        if(FileHandle::configs[CONFIGS::DIR_DEBUG]=="1"){
            std::cout << "rep_path: " << rep_path << '\n';
            std::cout << "import path: "<< target << "\\" << filename << '\n';
            std::cout << "object hash: " << hash << '\n';
            std::cout << "exists: " << std::filesystem::exists(object_path) << '\n';
        }

        if (!std::filesystem::exists(object_path)) return INVALID_FILE;

        //compressed bytes extraction
        std::ifstream file0(object_path, std::ios::binary);
        std::vector<unsigned char> compressed_bytes ( (std::istreambuf_iterator<char>(file0) ), std::istreambuf_iterator<char>() );

        //populate vector<>bytes with uncompressed bytes from object
        std::string size_str = mdata::ReadMData(object_path.string(), alt_stream, "OG_SIZE");

        //DIR_DEBUG
        if(FileHandle::configs[CONFIGS::DIR_DEBUG]=="1"){
            std::cout << "OG_SIZE: [" << size_str << "]\n";
        }

        size_t original_size = std::stoull(size_str);
        std::vector<unsigned char> bytes;
        if(!compression::Uncompress(compressed_bytes, bytes, original_size)) return COMPRESSION_ERROR;

        //write bytes to target file
        target += "\\" + filename;
        HANDLE file = CreateFileA(target.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr); 
        if(file==INVALID_HANDLE_VALUE) return INVALID_HANDLE;
        DWORD written;
        bool success = WriteFile(file, bytes.data(), static_cast<DWORD>(bytes.size()), &written, nullptr);
        FileInformation info{};
        info = mdata::GetWindowsFileId(file);
        CloseHandle(file);

        //write sign metadata
        mdata::ApplyHeaderData(target, alt_stream, id, hash, "none:", "none:");
        //add to id_fileid map
        FileHandle::id_fileid_map[id].push_back(info);

        //debug
        if(FileHandle::configs[CONFIGS::DIR_DEBUG]=="1"){
            std::cout << "recalculated file path (id_fileid_map): "<<mdata::GetPathFromId(info)<<'\n';
        }

        if(!success && written != bytes.size()) return FAILURE;
        
        //if in vs_solution, modify "project".vcxproj to add item to solution.
        if(FileHandle::configs[CONFIGS::VS_SOLUTION]=="1"){
            FileHandle::modify_vcxproj(std::filesystem::current_path().string(), target);
        }
        return SUCCESS;
    }

    // ######## ndep push {VARS}
    RETURN UpdateRepositoryObject(const std::string& name, const std::string& source, const std::string& rep_path, const std::string& alt_stream){
        //get current hash from id_hash_map (saves the hash of repository objects, not original files.)
        auto it = FileHandle::id_hash_map.find(name);
        if(it==FileHandle::id_hash_map.end()) return INVALID_NAME;
        std::string hash = it->second;

        //get name of update source
        std::string source_name = mdata::ReadMData(source, alt_stream, "ID");

        //get hash of update source
        //we recalculate the hash instead of using the one in metadata cz were assuming the file contents changed, thus why the user is even running ndep push
        std::string source_hash = nhash::SHA256File(source);
        //check hash of update source
        if(source_hash=="UNSIGNED") return UNSIGNED;
        if(hash==source_hash) return UPTODATE;

        //match names of source and provided name
        if(name != source_name) return NAME_MATCH_ERROR;

        //make new repository object, we dont overwrite the old one incase we wanna add a revert function later and store all previous hashes. eventually adding commits and actually
        //nearing git architercture
        mdata::FileRep filerep;
        filerep.og_path = source;
        filerep.content_hash = source_hash;
        if(MakeRepositoryObject(filerep, rep_path, alt_stream)!=SUCCESS) return FAILURE;

        mdata::ApplyHeaderData(source, alt_stream, "", source_hash); //only change sourcehash in original metadata for now. ill change copyid and versionid once i actually use them

        FileHandle::id_hash_map[name] = source_hash; //update hash map to refer to new version
        return SUCCESS;
    }

    //ndep update {lib} {-flags: -v:(version_number)}
    RETURN UpdateImportedFile(std::string name, const std::string& alt_stream, const std::string& rep_path){
        std::filesystem::path cwd = std::filesystem::current_path();
        //  find given id and latest hash
        auto it = FileHandle::id_hash_map.find(name);
        if(it==FileHandle::id_hash_map.end()) return INVALID_NAME;
        std::string stored_hash = it->second;

        //find all signed files in cwd and segment them based on id
        std::vector<std::filesystem::path> cwd_libs = GetAllSignedFiles(cwd, alt_stream);
        std::string calculated_hash = "UNSIGNED";
        std::string lib_path = "";
        //  find first lib with given name and use it. everything else doesnt matter.
        for(auto& lib : cwd_libs){
            if(mdata::ReadMData(lib.string(), alt_stream, "ID")==name){
                std::cout<<"Found: "<<lib.string()<<std::endl;
                calculated_hash = nhash::SHA256File(lib); //recalulate hash rather than using one given cz file mightve been changed and we wanna verify the changes.
                lib_path = lib.string();
                break;
            }
        }

        //see whether the 2 files are the same.
        if(stored_hash==calculated_hash) return UPTODATE;

        //update the file
        //  get object path from og hash. i might have to track this or smth cz finding it like this everytime is pissing me off
        std::string objname = stored_hash.substr(2);
        std::string dir = stored_hash.substr(0,2);
        std::filesystem::path object_path = std::filesystem::path(rep_path) / "objects" / dir / objname;
        
        //  get raw compressed object bytes.
        std::ifstream file0(object_path, std::ios::binary);
        std::vector<unsigned char> compressed_bytes((std::istreambuf_iterator<char>(file0)), std::istreambuf_iterator<char>());

        //  get original size from metadata of object. used to uncompress
        std::string size_str = mdata::ReadMData(object_path.string(), alt_stream, "OG_SIZE");

        //debug, i like the look of massive blocks of text in my terminal so i can act cool on instagram stories :3
        if(FileHandle::configs[CONFIGS::DIR_DEBUG]=="1"){
            std::cout << "rep_path: " << rep_path << '\n';
            std::cout << "stored hash: " << stored_hash << '\n';
            std::cout << "recalculated hash: " << calculated_hash << '\n';
            std::cout << "object_path: " << object_path.string() << '\n';
            std::cout << "exists: " << std::filesystem::exists(object_path) << '\n';
            std::cout << "OG_SIZE: [" << size_str << "]\n";
        }
        size_t original_size = std::stoull(size_str);

        //uncompress and get raw bytes
        std::vector<unsigned char> bytes;
        if(!compression::Uncompress(compressed_bytes, bytes, original_size)) return COMPRESSION_ERROR;

        //use filepath of given file, again, for now imported files arent signed properly so ill fix that next update, probs tmrw 
        HANDLE file = CreateFileA(lib_path.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
        if(file==INVALID_HANDLE_VALUE) return INVALID_HANDLE;
        DWORD written;
        bool success = WriteFile(file, bytes.data(), static_cast<DWORD>(bytes.size()), &written, nullptr);
        CloseHandle(file);

        mdata::ApplyHeaderData(lib_path, alt_stream, name, "a", "none:", "none:"); //we were using `calculated_hash` which was the content hash before the update, now its just `a`->
        //so its not null and mdata::ReadMData doesnt return `UNSIGNED` which will fuck over calcCWDHash cz it relies on it being signed already

        calcCWDHash(alt_stream); //actual recalculated hash after the update, also recalculates everything in that directory to keep everything uptodate
        
        if(success && written==bytes.size()) return SUCCESS;
        return FAILURE;
    }

    //`ndep rename {oldname} {newname}` renames every tracked copy of {oldname}
    RETURN renameLibrary(const std::string& old_name,const std::string& name, const std::string& alt_stream){
        //check whether old name exists in both data indexes.
        auto it =  FileHandle::id_hash_map.find(old_name);
        if(it==FileHandle::id_hash_map.end()) return INVALID_NAME;

        auto it2 =  FileHandle::id_fileid_map.find(old_name);
        if(it2==FileHandle::id_fileid_map.end()) return INVALID_NAME;

        //check if new name exists. return if yes
        if(FileHandle::id_hash_map.find(name)!=FileHandle::id_hash_map.end())return INVALID_NAME;
        if(FileHandle::id_fileid_map.find(name)!=FileHandle::id_fileid_map.end())return INVALID_NAME;

        std::string path;
        //loop through each entry of fileinformation.
        for(const auto& info : it2->second){
            path = mdata::GetPathFromId(info);
            if(mdata::ReadMData(path, alt_stream, "ID")==old_name){
                if(!mdata::WriteMData(path, alt_stream, "ID", name)) return INVALID_FILE;
            }
        }

        auto hash = it->second;
        auto fileids = std::move(it2->second);

        FileHandle::id_hash_map.erase(it);
        FileHandle::id_fileid_map.erase(it2);

        //change name in both maps.
        FileHandle::id_hash_map.emplace(name, std::move(hash));
        FileHandle::id_fileid_map.emplace(name, std::move(fileids)); //move not copy, i physically dont know the difference, moving is just copying but deleting the og copy, but what do i know

        return SUCCESS;
        
    }

    std::string findAllLibraryPaths(const std::string& name){
        auto it = FileHandle::id_fileid_map.find(name);
        if(FileHandle::id_hash_map.find(name)==FileHandle::id_hash_map.end())return "nth found \n";
        if(it==FileHandle::id_fileid_map.end())return "nth found \n";

        std::string returnstr;
        for(const auto& info : it->second){
            returnstr += mdata::GetPathFromId(info) + '\n';
        }

        return returnstr;
        
    }

    RETURN AddFileToGroup(const std::string& name, const std::string& group){
        bool newGroup = false; //for fancy return codes.
        //check for existance of id + existance of group.
        auto id = FileHandle::id_hash_map.find(name);
        if(id==FileHandle::id_hash_map.end()) return INVALID_NAME;

        auto grp = FileHandle::group_id_map.find(group);
        if(grp==FileHandle::group_id_map.end()) newGroup=true;

        //check for existance of id in group.
        auto it = FileHandle::group_id_map[group].find(id->first);
        if(it!=FileHandle::group_id_map[group].end()) return FILE_EXISTS;

        //append tag to group / new group
        FileHandle::group_id_map[group][id->first] = 1;
        //if(newGroup) return NEW_GROUP;
        return SUCCESS;
    }

    RETURN ImportGroup(const std::string& group, const std::string& rep_path, const std::string& alt_stream){
        auto grp = FileHandle::group_id_map.find(group);
        if(grp==FileHandle::group_id_map.end()) return INVALID_NAME;
        RETURN code = FAILURE;

        for(auto& [id, placeholder] : FileHandle::group_id_map[group]){
            code = ImportRepositoryObject(id, std::filesystem::current_path().string(), rep_path, alt_stream);
        }
        return code;
    }

    RETURN UpdateGroup(const std::string& group, const std::string& rep_path, const std::string& alt_stream){
        auto grp = FileHandle::group_id_map.find(group);
        if(grp==FileHandle::group_id_map.end()) return INVALID_NAME;
        RETURN code = FAILURE;

        for(auto& [id, placeholder] : grp->second){
            code = UpdateImportedFile(id, alt_stream, rep_path);
        }
        return code;
    }
}