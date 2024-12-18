#include "config.hpp"
#include <eosio/chain/genesis_intrinsics.hpp>

#define genesis_intrinsics eos_genesis_intrinsics
#include <chain/genesis_intrinsics.cpp>
#undef genesis_intrinsics

namespace eosio { namespace chain {

inline const std::vector<const char*> init_genesis_intrinsics()
{
    std::vector<const char*> genesis_intrinsics = eos_genesis_intrinsics;
    genesis_intrinsics.emplace_back("do_plugin_function");

    return genesis_intrinsics;
}

const std::vector<const char*> genesis_intrinsics = init_genesis_intrinsics();

}}

