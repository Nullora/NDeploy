#pragma once
#include<vector>
#include<zlib.h>


namespace compression{
    bool Compress(const std::vector<unsigned char>& input, std::vector<unsigned char>& output){
        uLong source_size = static_cast<uLong>(input.size());
        uLongf compressed_size = compressBound(source_size);
        output.resize(compressed_size);
        int result = compress2(output.data(), &compressed_size, input.data(),source_size, Z_BEST_COMPRESSION);
        if(result != Z_OK){
            return false;
        }
        output.resize(compressed_size);
        return true;
    }

    //pure ai, fuck compression and zlib im not reading their 30 year old docs
    static bool Uncompress(const std::vector<unsigned char>& input, std::vector<unsigned char>& output, size_t original_size){
        uLongf dest_size = static_cast<uLongf>(original_size);

        output.resize(dest_size);

        int result = uncompress(output.data(), &dest_size, input.data(), static_cast<uLong>(input.size()));

        if(result != Z_OK){
            output.clear();
            return false;
        }

        output.resize(dest_size);
        return true;
    }


}