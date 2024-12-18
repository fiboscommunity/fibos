#pragma once

#include <eosio/chain/webassembly/common.hpp>
#include <fc/variant.hpp>

namespace eosio {
namespace chain {
    struct global_var {
        static global_var& instance();
        using plugin_func_callback = std::function<int(const apply_context&, legacy_span<char> ret, legacy_span<char> data)>;
        std::map<std::string, plugin_func_callback> plugin_map;
        bool add_callback(std::string plugin_name, std::string fucntion_name, plugin_func_callback call_back_func);
    };
}
}
