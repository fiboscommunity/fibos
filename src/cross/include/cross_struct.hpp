#pragma once
#include <fc/reflect/reflect.hpp>
namespace fibos {
struct block_header {
    fc::sha256 hash;

    std::string parent_hash;
    std::string uncle_hash;

    std::string coinbase;

    std::string state_root_hash;
    std::string transaction_root_hash;
    std::string receipt_root_hash;
    std::string blooms;

    std::string difficulty;
    std::string number;

    std::string gas_limit;
    std::string gas_used;

    std::string timestamp;

    std::string extra_hash;
    std::string mix_hash;

    std::string nonce;
};

struct nonce_header {
    std::string number;
    std::string difficulty;
    std::string mix_hash;
    std::string nonce;

    std::string hash_without_nonce;
};

typedef fc::array<char, 65> public_key_point_data;
typedef fc::array<char, 64> split_public_key_data;
};
FC_REFLECT(fibos::block_header, (hash)(parent_hash)(uncle_hash)(coinbase)(state_root_hash)(transaction_root_hash)(receipt_root_hash)(blooms)(difficulty)(number)(gas_limit)(gas_used)(timestamp)(extra_hash)(mix_hash)(nonce))
FC_REFLECT(fibos::nonce_header, (number)(difficulty)(mix_hash)(nonce)(hash_without_nonce))
