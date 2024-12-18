#include "config.hpp"

#define only_bill_first_authorizer _only_bill_first_authorizer
#include <eosio/chain/protocol_feature_manager.hpp>
#undef only_bill_first_authorizer

#define only_bill_first_authorizer _only_bill_first_authorizer) || true )                                                \
    {                                                                                                                    \
        if (trx.actions.size() == 1 && trx.actions.front().authorization.size() == 1                                     \
            && trx.actions.front().account == chain::config::eosio_cross_name && trx.actions.front().name == "sign"_n) { \
            const auto& ap = control.active_producers();                                                                 \
            const auto& actor = trx.first_authorizer();                                                                  \
            bool is_producer = false;                                                                                    \
                                                                                                                         \
            for (const auto& p : ap.producers)                                                                           \
                if (actor == p.producer_name) {                                                                          \
                    is_producer = true;                                                                                  \
                    break;                                                                                               \
                }                                                                                                        \
                                                                                                                         \
            if (!is_producer)                                                                                            \
                bill_to_accounts.insert(actor);                                                                          \
            else                                                                                                         \
                bill_to_accounts.insert(chain::config::null_account_name);                                               \
        }                                                                                                                \
    }                                                                                                                    \
                                                                                                                         \
    if (!bill_to_accounts.empty()) {                                                                                     \
        if (*bill_to_accounts.begin() == chain::config::null_account_name)                                               \
            bill_to_accounts.clear();                                                                                    \
    } else if( control.is_builtin_activated(builtin_protocol_feature_t::_only_bill_first_authorizer

#include <chain/transaction_context.cpp>
