#pragma once
#include <Windows.h>
#include <bcrypt.h>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>

namespace nhash {

    std::string SHA256(const std::string& input) { //honestly i followed a guide and used ai to implement this, idk how encryption works let alone bcrypt 
        BCRYPT_ALG_HANDLE algorithm = nullptr;
        BCRYPT_HASH_HANDLE hash_handle = nullptr;

        NTSTATUS status = BCryptOpenAlgorithmProvider(&algorithm, BCRYPT_SHA256_ALGORITHM, nullptr, 0);
        if (status < 0) return "";

        DWORD hash_size = 32;
        std::vector<BYTE> hash_bytes(hash_size);

        status = BCryptCreateHash(algorithm, &hash_handle, nullptr, 0, nullptr, 0, 0);
        if (status < 0) {
            BCryptCloseAlgorithmProvider(algorithm, 0);
            return "";
        }

        status = BCryptHashData(hash_handle, reinterpret_cast<PUCHAR>(const_cast<char*>(input.data())), static_cast<ULONG>(input.size()), 0);
        if (status >= 0) {
            status = BCryptFinishHash(hash_handle, hash_bytes.data(), hash_size, 0);
        }
        BCryptDestroyHash(hash_handle);
        BCryptCloseAlgorithmProvider(algorithm, 0);

        if (status < 0) return "";

        std::ostringstream result;
        for (BYTE byte : hash_bytes) {
            result << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
        }
        return result.str();
    }
}