#include "config.hpp"

#define find_apply_handler eos_find_apply_handler
#include <chain/controller.cpp>

namespace eosio {
namespace chain {
    const apply_handler* controller_find_apply_handler(const controller* self, account_name contract, scope_name scope, action_name act)
    {
        return self->find_apply_handler(contract, scope, act);
    }
}
}