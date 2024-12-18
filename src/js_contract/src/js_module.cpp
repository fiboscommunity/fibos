#include "config.hpp"

#define private public
#include <eosio/chain/transaction_context.hpp>
#undef private

#include "../include/js_module.h"
#include "js_value.h"

#include <eosio/chain_plugin/chain_plugin.hpp>
#include <eosio/chain/exceptions.hpp>
#include <eosio/chain/controller.hpp>

#include "Buffer.h"
#include "SandBox.h"
#include "ifs/fs.h"
#include "ifs/json.h"
#include "Timer.h"

using namespace fibjs;

namespace eosio {
namespace chain {
    bool js_module_interface::g_enable = true;
    fibjs::Isolate* js_module_interface::g_isolate = NULL;
    js_module_interface* js_module_interface::g_module = NULL;

    bool g_isMainnet;

    struct fix_data {
        const char* id;
        int32_t fix;
    };

    static fix_data s_fix_datas[] = {
        { "282c88e84947d14b727c367fe7ab96813b1f037dee0c1147938d7f3d24021a81", 81 },
        { "5327817ffa0694236f43e7b5c4eb17b55b937577ad3d9d4dd244dafb84ff8407", 13 },
        { "9ded348fa55bd30dc72e7b0d7950a5dcef627523e4987161c2f2eb92ef2c0621", 16 },
        { "0b56e0a4b0e741fdd07a6ec3409c190b344741a2d9e7f214c236de28f84d0a6d", 26 },
        { "b8d5a9983e5b63ab94eb5f6d9b6800ff427be2a742e4d23c34cb7cc6a0d3ab03", 29 },
        { "72b1e9fa23a3c235b881f4710f646d425d3d3d27a4c2ada3c8e6d12cbe93d365", 32 },
        { "7e2b0f51e07289fa5bfae48cdc393e49cd70bb29fbd62b493b4da85c8b6ccab0", 35 },
        { "d8ab48b91cf1a65367bb06dd425c7c224262245f8d86141399b1c989f78b0774", 38 },
        { "bb3f1c45dd19acfb41d2b90699947da52523e10da1925e80b2c79a4f514fba37", 19 },
        { "2072e2191585925dfcb73f9513c2ce331bfc15146b061ecea9a8829471555307", 75 },
        { "bcbe28bbfbefbc6843fbdbeb84b7b5ca2979f246f492c783482151148519bfc7", 91 }
    };

    inline double timeout()
    {
        double tm;

        if (g_now_context->control.is_speculative_block()) {
            tm = (g_now_context->trx_context._deadline - fc::time_point::now()).count() / 1000.0;
            if (tm < 2)
                tm = 2;
        } else
            tm = 5000;

        return tm;
    }

    void js_module_interface::load_state()
    {
        if (m_state.IsEmpty()) {
            int32_t itr;

            try {
                exlib::string str;
                v8::Local<v8::Value> v;

                itr = g_now_context->db_find_i64(g_now_context->get_receiver(), g_now_context->get_receiver(), "state"_n, 0);
                if (itr > -1) {
                    int sz = g_now_context->db_get_i64(itr, NULL, 0);
                    char value[sz];
                    g_now_context->db_get_i64(itr, value, sz);
                    value[sz] = '\0';
                    exlib::string s(value);
                    json_base::decode(s, v);

                    if (!v.IsEmpty() && v->IsObject()) {
                        if (g_now_context->get_receiver() == "dicefobetone"_n && g_now_context->get_action().name == "updateresult"_n && g_isMainnet) {
                            size_t sz = ARRAYSIZE(s_fix_datas);
                            for (size_t i = 0; i < sz; i++)
                                if (g_now_context->trx_context.packed_trx.id().str() == s_fix_datas[i].id) {
                                    v8::Local<v8::Object>::Cast(v)->Set(js_module_interface::g_isolate->context(), js_module_interface::g_isolate->NewString("from_here"),
                                        v8::Number::New(js_module_interface::g_isolate->m_isolate, s_fix_datas[i].fix));
                                    break;
                                }
                        }

                        m_state.Reset(js_module_interface::g_isolate->m_isolate, v8::Local<v8::Object>::Cast(v));
                    }
                }
            } catch (const chain_exception& e) {
            }
        }
    }

    void js_module_interface::save_state(apply_context& context)
    {
        if (!m_state.IsEmpty()) {
            int32_t itr;
            result_t hr;

            exlib::string str;

            hr = json_base::encode(m_state.Get(js_module_interface::g_isolate->m_isolate), str);
            m_state.Reset();

            EOS_ASSERT(hr >= 0, js_serialization_error, getResultMessage(hr));

            itr = context.db_find_i64(context.get_receiver(), context.get_receiver(), "state"_n, 0);
            if (itr > -1)
                context.db_update_i64(itr, context.get_receiver(), str.c_str(), str.length());
            else
                context.db_store_i64(context.get_receiver(), "state"_n, context.get_receiver(), 0, str.c_str(), str.length());
        }
    }

    void js_module_interface::apply(apply_context& context)
    {
        class apply_scope {
        public:
            apply_scope()
            {
            }
            ~apply_scope()
            {
                fs_base::clearZipFS("");

                g_module = NULL;
                g_now_context = NULL;
            }
        };

        result_t hr;
        v8::Locker locker(js_module_interface::g_isolate->m_isolate);
        v8::HandleScope handle_scope(js_module_interface::g_isolate->m_isolate);

        m_state.Reset();

        context.trx_context.pause_billing_timer();
        hr = fs_base::setZipFS(m_fname, m_code);
        context.trx_context.resume_billing_timer();

        EOS_ASSERT(hr >= 0, js_serialization_error, getResultMessage(hr));

        obj_ptr<SandBox> _sbox = new SandBox(false);

        g_now_context = &context;
        g_module = this;

        apply_scope ac;

        JSValue v;
        {
            TryCatch try_catch;

            double tm = timeout();
            if (tm > 0) {
                TimeoutScope ts(tm);
                hr = ts.result(_sbox->require(m_fname + "$", "/", v));
            } else
                hr = CALL_E_TIMEOUT;

            EOS_ASSERT(hr >= 0, js_execution_error, GetException(try_catch, hr, false, true));
            EOS_ASSERT(v->IsObject(), js_execution_error, "The result of the module should be Object.");
        }

        const eosio::chain::abi_serializer* _abis = &m_abis;
        std::vector<v8::Local<v8::Value>> args;

        eosio::chain::abi_serializer abisEvent;
        if (context.get_receiver() != context.get_action().account) {
            abisEvent.set_abi(context.control.get_account(context.get_action().account).get_abi(), abi_serializer::create_yield_function(max_serialization_time));
            _abis = &abisEvent;
        }

        auto var = _abis->binary_to_variant(_abis->get_action_type(context.get_action().name), context.get_action().data, abi_serializer::create_yield_function(max_serialization_time));
        auto obj = var.get_object();

        for (fc::variant_object::iterator it = obj.begin(); it != obj.end(); it++)
            args.push_back(js_value(js_module_interface::g_isolate, it->value()));

        v8::Local<v8::Object> module = v8::Local<v8::Object>::Cast(v);
        v8::Local<v8::Context> bc_context = js_module_interface::g_isolate->context();
        JSValue func;

        if (context.get_receiver() == context.get_action().account) {
            func = module->Get(bc_context, js_module_interface::g_isolate->NewString(context.get_action().name.to_string()));
            EOS_ASSERT(!IsEmpty(func), js_execution_error, context.get_action().name.to_string() + " is not found.");
        } else {
            func = module->Get(bc_context, js_module_interface::g_isolate->NewString(("on_" + context.get_action().name.to_string())));
            if (IsEmpty(func))
                return;
        }

        EOS_ASSERT(func->IsFunction(), js_execution_error, context.get_action().name.to_string() + " is not a Function.");

        JSValue result;
        {
            TryCatch try_catch;

            double tm = timeout();
            if (tm > 0) {
                TimeoutScope ts(tm);
                result = v8::Local<v8::Function>::Cast(func)->Call(bc_context, v8::Undefined(js_module_interface::g_isolate->m_isolate), (int32_t)args.size(), args.data());
                hr = ts.result(result.IsEmpty() ? CALL_E_JAVASCRIPT : 0);
            } else
                hr = CALL_E_TIMEOUT;

            EOS_ASSERT(hr >= 0, js_execution_error, GetException(try_catch, hr, false, true));
        }

        if (!result->IsUndefined()) {
            auto rt = _abis->get_action_result_type(context.get_action().name);
            if (!rt.empty())
                context.action_return_value = _abis->variant_to_binary(rt,
                    fc_value(js_module_interface::g_isolate, result), abi_serializer::create_yield_function(max_serialization_time));
        }

        save_state(context);
    }

} // namespace chain
} // namespace eosio
