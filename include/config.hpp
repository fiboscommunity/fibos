#pragma once

#ifdef EOSIO_EOS_VM_RUNTIME_ENABLED

#define system_account_name eos_system_account_name
#define null_account_name eos_null_account_name
#define producers_account_name eos_producers_account_name
#define eosio_auth_scope eos_eosio_auth_scope
#define eosio_all_scope eos_eosio_all_scope
#define eosio_any_name eos_eosio_any_name
#define eosio_code_name eos_eosio_code_name

#include <eosio/chain/config.hpp>

#undef system_account_name
#undef null_account_name
#undef producers_account_name
#undef eosio_auth_scope
#undef eosio_all_scope
#undef eosio_any_name
#undef eosio_code_name

namespace eosio {
namespace chain {
    namespace config {

        class names {
        public:
            static name& _system_account_name()
            {
                static name v = { "eosio"_n };
                return v;
            }
            static name& _null_account_name()
            {
                static name v = { "eosio.null"_n };
                return v;
            }
            static name& _producers_account_name()
            {
                static name v = { "eosio.prods"_n };
                return v;
            }
            static name& _token_account_name()
            {
                static name v = { "eosio.token"_n };
                return v;
            }

            static name& _eosio_auth_scope()
            {
                static name v = { "eosio.auth"_n };
                return v;
            }
            static name& _eosio_all_scope()
            {
                static name v = { "eosio.all"_n };
                return v;
            }

            static name& _eosio_any_name()
            {
                static name v = { "eosio.any"_n };
                return v;
            }
            static name& _eosio_code_name()
            {
                static name v = { "eosio.code"_n };
                return v;
            }
            static name& _eosio_cross_name()
            {
                static name v = { "eosio.cross"_n };
                return v;
            }
        };

#define system_account_name names::_system_account_name()
#define null_account_name names::_null_account_name()
#define producers_account_name names::_producers_account_name()
#define token_account_name names::_token_account_name()
#define eosio_auth_scope names::_eosio_auth_scope()
#define eosio_all_scope names::_eosio_all_scope()
#define eosio_any_name names::_eosio_any_name()
#define eosio_code_name names::_eosio_code_name()
#define eosio_cross_name names::_eosio_cross_name()

    }
}
} // namespace eosio::chain::config

#endif
