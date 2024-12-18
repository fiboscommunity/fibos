#include "config.hpp"
#include "../include/global_var.hpp"

#define interface eos_interface
#define private public
#include <eosio/chain/webassembly/interface.hpp>
#undef private
#undef interface

#include <eosio/chain/webassembly/preconditions.hpp>

#define REGISTER_LEGACY_HOST_FUNCTION(NAME, ...)                                                                        \
    static host_function_registrator<&interface::NAME, legacy_static_check_wl_args, context_aware_check, ##__VA_ARGS__> \
        NAME##_registrator_impl()                                                                                       \
    {                                                                                                                   \
        return { BOOST_HANA_STRING("env"), BOOST_HANA_STRING(#NAME) };                                                  \
    }                                                                                                                   \
    inline static auto NAME##_registrator = NAME##_registrator_impl();

namespace eosio {
namespace chain {
    template <auto HostFunction, typename... Preconditions>
    struct host_function_registrator {
        template <typename Mod, typename Name>
        constexpr host_function_registrator(Mod mod_name, Name fn_name)
        {
            using rhf_t = eos_vm_host_functions_t;
            rhf_t::add<HostFunction, Preconditions...>(mod_name.c_str(), fn_name.c_str());
        }
    };

    global_var& global_var::instance()
    {
        static global_var _global_var;
        return _global_var;
    }

    bool global_var::add_callback(std::string plugin_name, std::string fucntion_name, plugin_func_callback call_back_func)
    {
        std::string key_name = plugin_name;
        key_name.append("/", 1);
        key_name.append(fucntion_name);
        auto result = plugin_map.emplace(std::make_pair(key_name, call_back_func));
        return result.second;
    }

    namespace webassembly {
        class interface : public eos_interface {
        public:
            interface(apply_context& ctx)
                : eos_interface(ctx)
            {
            }

            inline static constexpr size_t max_assert_message = 1024;
            int32_t do_plugin_function(null_terminated_ptr plugin_name, legacy_span<char> ret, legacy_span<char> data) const
            {
                const size_t sz1 = strnlen(plugin_name.data(), max_assert_message);
                std::string plugin_name_str(plugin_name.data(), sz1);
                auto iter = global_var::instance().plugin_map.find(plugin_name_str);
                EOS_ASSERT(iter != global_var::instance().plugin_map.end(), wasm_execution_error, "Can't find plugin function to execute!");
                return iter->second(context, std::move(data), std::move(ret));
            }
        };

        REGISTER_LEGACY_HOST_FUNCTION(do_plugin_function);
    }
}
}