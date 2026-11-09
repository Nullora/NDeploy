//to be fully honest, most of this sha256 hasher is made with ai. im not gonna dive into cryptography, this was supposed to be a quick tool this is already too far lol
#pragma once
#include <Windows.h>
#include <bcrypt.h>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <filesystem>

namespace nhash {

    class SHA256Hasher {
        BCRYPT_ALG_HANDLE algorithm = nullptr;
        BCRYPT_HASH_HANDLE hash_handle = nullptr;
        bool valid = false;
        bool finished = false;

    public:

        SHA256Hasher() {
            NTSTATUS status = BCryptOpenAlgorithmProvider(&algorithm, BCRYPT_SHA256_ALGORITHM, nullptr, 0);

            if (status < 0) return;

            status = BCryptCreateHash(algorithm, &hash_handle, nullptr, 0, nullptr, 0, 0);

            if (status < 0) {
                BCryptCloseAlgorithmProvider(algorithm, 0);
                algorithm = nullptr;
                return;
            }

            valid = true;
        }

        ~SHA256Hasher() {
            if (hash_handle)
                BCryptDestroyHash(hash_handle);

            if (algorithm)
                BCryptCloseAlgorithmProvider(algorithm, 0);
        }

        bool Update(const BYTE* data, size_t size) {
            if (!valid || finished) return false;

            while (size > 0) {
                ULONG chunk_size = static_cast<ULONG>(size > ULONG_MAX ? ULONG_MAX : size); //clamp size to ulong_max

                NTSTATUS status = BCryptHashData(hash_handle, const_cast<PUCHAR>(data), chunk_size,0);

                if (status < 0) return false;

                data += chunk_size;
                size -= chunk_size;
            }

            return true;
        }

        bool Finish(std::string& result) {
            if (!valid || finished) return false;
            BYTE hash_bytes[32];
            NTSTATUS status = BCryptFinishHash(hash_handle, hash_bytes, sizeof(hash_bytes), 0);
            if (status < 0) return false;

            std::ostringstream output;

            for (BYTE byte : hash_bytes) {
                output << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
            }

            result = output.str();
            finished = true;

            return true;
        }
    };

    // hash bytes
    std::string SHA256(const BYTE* data, size_t size) {
        SHA256Hasher hasher;
        std::string result;

        if (!hasher.Update(data, size)) return "";
        if (!hasher.Finish(result)) return "";

        return result;
    }

    // incremental hashing for big files.
    std::string SHA256File(const std::filesystem::path& path) {
        std::ifstream file(path, std::ios::binary);

        if (!file) return "";

        SHA256Hasher hasher;

        std::vector<BYTE> buffer(64 * 1024);

        while (file) {
            file.read(reinterpret_cast<char*>(buffer.data()), buffer.size());

            std::streamsize bytes_read = file.gcount();

            if (bytes_read > 0) {
                if (!hasher.Update(buffer.data(), static_cast<size_t>(bytes_read))) return "";
            }
        }

        if (!file.eof()) return "";

        std::string result;

        if (!hasher.Finish(result)) return "";

        return result;
    }
}
