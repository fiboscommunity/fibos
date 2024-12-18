#include "config.hpp"

#include <eosio/chain/controller.hpp>

namespace eosio {
namespace chain {
    const apply_handler* controller_find_apply_handler(const controller* self, account_name contract, scope_name scope, action_name act);
    const apply_handler* controller::find_apply_handler(account_name contract, scope_name scope, action_name act) const
    {
        if (contract == eosio::chain::config::system_account_name && contract == scope)
            contract = scope = { "eosio"_n };

        return controller_find_apply_handler(this, contract, scope, act);
    }
}
}