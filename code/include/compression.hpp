#pragma once
#include<vector>
#include<zlib.h>

using namespace std;

namespace compression{
    bool Compress(const vector<unsigned char>& input, vector<unsigned char>& output){
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
}