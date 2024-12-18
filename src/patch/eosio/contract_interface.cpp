#define EOSIO_EOS_VM_RUNTIME_ENABLED

#ifdef __x86_64__
#define EOSIO_EOS_VM_JIT_RUNTIME_ENABLED
#endif

#define wasm_interface wasm_interface_old
#include <chain/wasm_interface.cpp>
#undef wasm_interface

#include "config.hpp"
#include "../../js_contract/include/js_module.h"
#include <fc/io/json.hpp>

namespace eosio {
namespace chain {
    apply_context* g_now_context;
    fc::microseconds max_serialization_time;

    struct contract_interface_impl;

    class wasm_interface {
    public:
        enum class vm_type {
            eos_vm,
            eos_vm_jit,
            eos_vm_oc
        };

        // return string description of vm_type
        static std::string vm_type_string(vm_type vmtype)
        {
            switch (vmtype) {
            case vm_type::eos_vm:
                return "eos-vm";
            case vm_type::eos_vm_oc:
                return "eos-vm-oc";
            default:
                return "eos-vm-jit";
            }
        }

        enum class vm_oc_enable {
            oc_auto,
            oc_all,
            oc_none
        };

        inline static bool test_disable_tierup = false; // set by unittests to test tierup failing

        wasm_interface(vm_type vm, vm_oc_enable eosvmoc_tierup, const chainbase::database& d, const std::filesystem::path data_dir, const eosvmoc::config& eosvmoc_config, bool profile);
        ~wasm_interface();

#ifdef EOSIO_EOS_VM_OC_RUNTIME_ENABLED
        // initialize exec per thread
        void init_thread_local_data();

        // returns true if EOS VM OC is enabled
        bool is_eos_vm_oc_enabled() const;
#endif

        // call before dtor to skip what can be minutes of dtor overhead with some runtimes; can cause leaks
        void indicate_shutting_down();

        // validates code -- does a WASM validation pass and checks the wasm against EOSIO specific constraints
        static void validate(const controller& control, const bytes& code);

        // indicate that a particular code probably won't be used after given block_num
        void code_block_num_last_used(const digest_type& code_hash, const uint8_t& vm_type, const uint8_t& vm_version, const uint32_t& block_num);

        // indicate the current LIB. evicts old cache entries
        void current_lib(const uint32_t lib);

        // Calls apply or error on a given code
        void apply(const digest_type& code_hash, const uint8_t& vm_type, const uint8_t& vm_version, apply_context& context);

        // Returns true if the code is cached
        bool is_code_cached(const digest_type& code_hash, const uint8_t& vm_type, const uint8_t& vm_version) const;

        // If substitute_apply is set, then apply calls it before doing anything else. If substitute_apply returns true,
        // then apply returns immediately. Provided function must be multi-thread safe.
        std::function<bool(const digest_type& code_hash, uint8_t vm_type, uint8_t vm_version, apply_context& context)> substitute_apply;

    private:
        vm_oc_enable eosvmoc_tierup;
        unique_ptr<struct contract_interface_impl> my;
    };

    struct contract_interface_impl : public wasm_interface_impl {
        contract_interface_impl(wasm_interface::vm_type vm, wasm_interface::vm_oc_enable eosvmoc_tierup, const chainbase::database& d,
            const std::filesystem::path data_dir, const eosvmoc::config& eosvmoc_config, bool profile)
            : wasm_interface_impl((wasm_interface_old::vm_type)vm, (wasm_interface_old::vm_oc_enable)eosvmoc_tierup,
                d, data_dir, eosvmoc_config, profile)
        {
        }

        static std::unique_ptr<wasm_instantiated_module_interface> instantiate_module(const shared_string& code, apply_context& context, int64_t abi_sequence)
        {
            if (js_module_interface::validate(code))
                return js_module_interface::instantiate_module(code, context, abi_sequence);

            FC_THROW_EXCEPTION(wasm_execution_error, "code is not wasm, js");
        }

        const std::unique_ptr<wasm_instantiated_module_interface>& get_contract_module(const digest_type& code_hash, const uint8_t& vm_type,
            const uint8_t& vm_version, apply_context& context)
        {
            auto it = wasm_instantiation_cache.find(std::make_tuple(code_hash, vm_type, vm_version));
            if (it != wasm_instantiation_cache.end() && dynamic_cast<contract_interface*>(it->module.get()) == nullptr)
                return it->module;

            const code_object* codeobject = &context.db.get<code_object, by_code_hash>(boost::make_tuple(code_hash, vm_type, vm_version));
            auto code = codeobject->code;
            if (code.size() > 4 && *(int32_t*)code.data() == 0x6d736100)
                return get_instantiated_module(code_hash, vm_type, vm_version, context.trx_context);

            const auto& account_sequence = context.db.get<account_metadata_object, by_name>(context.get_receiver());
            if (it != wasm_instantiation_cache.end()) {
                std::unique_ptr<contract_interface>& _module = *(std::unique_ptr<contract_interface>*)&it->module;
                if (account_sequence.abi_sequence != _module->m_abi_sequence) {
                    wasm_instantiation_cache.modify(it, [&](auto& c) {
                        c.module = instantiate_module(code, context, account_sequence.abi_sequence);
                    });
                }
            } else {
                it = wasm_instantiation_cache
                         .emplace(wasm_interface_impl::wasm_cache_entry {
                             .code_hash = code_hash,
                             .last_block_num_used = UINT32_MAX,
                             .module = instantiate_module(code, context, account_sequence.abi_sequence),
                             .vm_type = vm_type,
                             .vm_version = vm_version })
                         .first;
            }
            return it->module;
        }
    };

    wasm_interface::wasm_interface(vm_type vm, vm_oc_enable eosvmoc_tierup, const chainbase::database& d,
        const std::filesystem::path data_dir, const eosvmoc::config& eosvmoc_config, bool profile)
        : eosvmoc_tierup(eosvmoc_tierup)
        , my(new contract_interface_impl(vm, eosvmoc_tierup, d, data_dir, eosvmoc_config, profile))
    {
    }

    wasm_interface::~wasm_interface()
    {
    }

#ifdef EOSIO_EOS_VM_OC_RUNTIME_ENABLED
    void wasm_interface::init_thread_local_data()
    {
        // OC tierup and OC runtime are mutually exclusive
        if (my->eosvmoc) {
            my->eosvmoc->init_thread_local_data();
        } else if (my->wasm_runtime_time == wasm_interface::vm_type::eos_vm_oc && my->runtime_interface) {
            my->runtime_interface->init_thread_local_data();
        }
    }
#endif

    // validates code -- does a WASM validation pass and checks the wasm against EOSIO specific constraints
    void wasm_interface::validate(const controller& control, const bytes& code)
    {
        if (code.size() > 4 && *(int32_t*)code.data() == 0x6d736100) {
            wasm_interface_old::validate(control, code);
            return;
        }

        if (js_module_interface::validate(code))
            return;

        EOS_ASSERT(false, chain_exception, "unsupport contract.");
    }

    void wasm_interface::code_block_num_last_used(const digest_type& code_hash, const uint8_t& vm_type, const uint8_t& vm_version, const uint32_t& block_num)
    {
        my->code_block_num_last_used(code_hash, vm_type, vm_version, block_num);
    }

    void wasm_interface::current_lib(const uint32_t lib)
    {
        my->current_lib(lib);
    }

    // Calls apply or error on a given code
    void wasm_interface::apply(const digest_type& code_hash, const uint8_t& vm_type,
        const uint8_t& vm_version, apply_context& context)
    {
        my->get_contract_module(code_hash, vm_type, vm_version, context)->apply(context);
    }

    bool wasm_interface::is_code_cached(const digest_type& code_hash, const uint8_t& vm_type, const uint8_t& vm_version) const
    {
        return my->is_code_cached(code_hash, vm_type, vm_version);
    }

#ifdef EOSIO_EOS_VM_OC_RUNTIME_ENABLED
    bool wasm_interface::is_eos_vm_oc_enabled() const
    {
        return my->is_eos_vm_oc_enabled();
    }
#endif

    std::istream& operator>>(std::istream& in, wasm_interface::vm_type& runtime)
    {
        return operator>>(in, *(wasm_interface_old::vm_type*)&runtime);
    }
} // namespace chain
} // namespace eosio
