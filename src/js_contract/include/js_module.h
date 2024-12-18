#pragma once

#include "contract_interface.h"

namespace eosio {
namespace chain {
#define JS_ERROR_BASE 3500000

    FC_DECLARE_DERIVED_EXCEPTION(js_exception, chain_exception, JS_ERROR_BASE, "JavaScript Exception")

    FC_DECLARE_DERIVED_EXCEPTION(js_execution_error, js_exception, JS_ERROR_BASE + 1, "Runtime Error Processing JavaScript")

    FC_DECLARE_DERIVED_EXCEPTION(js_serialization_error, js_exception, JS_ERROR_BASE + 2, "Serialization Error Processing JavaScript")

    class js_module_interface : public contract_interface {
    public:
        js_module_interface(const shared_string& code, apply_context& context, int64_t abi_sequence)
            : contract_interface(code, context, abi_sequence)
            , m_fname("/" + context.get_receiver().to_string())
        {
        }

    public:
        virtual void apply(apply_context& context);
        static std::unique_ptr<wasm_instantiated_module_interface> instantiate_module(const shared_string& code, apply_context& context, int64_t abi_sequence)
        {
            return std::make_unique<js_module_interface>(code, context, abi_sequence);
        }

        template <typename T>
        static bool validate(const T& code)
        {
            return g_enable && code.size() > 4 && *(int32_t*)code.data() == 0x04034b50;
        }

    public:
        void load_state();
        void save_state(apply_context& context);

    public:
        exlib::string m_fname;
        v8::Global<v8::Object> m_state;

    public:
        static bool g_enable;
        static fibjs::Isolate* g_isolate;
        static js_module_interface* g_module;
    };
}
}
