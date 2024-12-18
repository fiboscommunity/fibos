#pragma once

#include "fibjs.h"
#include "Buffer.h"

#include <eosio/chain/wasm_interface.hpp>
#include <eosio/chain/webassembly/runtime_interface.hpp>
#include <eosio/chain/apply_context.hpp>
#include <eosio/chain/abi_def.hpp>
#include <eosio/chain/abi_serializer.hpp>
#include <eosio/chain/account_object.hpp>

namespace eosio {
namespace chain {
    extern apply_context* g_now_context;
    extern fc::microseconds max_serialization_time;
    extern bool g_isMainnet;

    class contract_interface : public wasm_instantiated_module_interface {
    public:
        contract_interface(const shared_string& code, apply_context& context, int64_t abi_sequence)
            : m_code(new fibjs::Buffer((const char*)code.data(), code.size()))
            , m_abi(context.control.get_account(context.get_receiver()).get_abi())
            , m_abis(m_abi, abi_serializer::create_yield_function(max_serialization_time))
            , m_abi_sequence(abi_sequence)
        {
        }

    public:
        fibjs::obj_ptr<fibjs::Buffer> m_code;
        const eosio::chain::abi_def m_abi;
        const eosio::chain::abi_serializer m_abis;
        uint64_t m_abi_sequence;
    };
}
}
