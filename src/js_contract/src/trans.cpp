#include "config.hpp"

#include "fibjs.h"
#include "object.h"
#include "ifs/trans.h"
#include "js_value.h"
#include "../include/js_module.h"
#include <eosio/chain/action.hpp>
#include <eosio/chain/abi_serializer.hpp>

using namespace eosio::chain;

namespace fibjs {

DECLARE_MODULE(trans);

result_t fillAction(exlib::string account, exlib::string name,
    v8::Local<v8::Object> args, action& act)
{
    result_t hr;

    auto act_account = act.account.to_uint64_t();
    auto act_name = act.name.to_uint64_t();

    hr = string_to_name(account, act_account);
    if (hr < 0)
        return hr;

    hr = string_to_name(name, act_name);
    if (hr < 0)
        return hr;

    fc::variant v = fc_value(js_module_interface::g_isolate, args);

    try {
        abi_serializer abis(g_now_context->control.get_account(account_name(act_account)).get_abi(),
            abi_serializer::create_yield_function(max_serialization_time));

        act.data = abis.variant_to_binary(abis.get_action_type(action_name(act_name)), v, abi_serializer::create_yield_function(max_serialization_time));
        act.account = account_name(act_account);
        act.name = action_name(act_name);
    } catch (const chain_exception& e) {
        return CHECK_ERROR(Runtime::setError(e.to_detail_string()));
    } catch (const fc::exception& e) {
        return CHECK_ERROR(Runtime::setError(e.to_detail_string()));
    } catch (const std::exception& e) {
        return CHECK_ERROR(Runtime::setError(e.what()));
    } catch (...) {
        return CHECK_ERROR(Runtime::setError("unknown exception"));
    }

    return 0;
}

result_t fillAction(exlib::string account, exlib::string name,
    v8::Local<v8::Object> args, JSArray authorization, action& act)
{
    v8::Local<v8::String> actor = js_module_interface::g_isolate->NewString("actor");
    v8::Local<v8::String> permission = js_module_interface::g_isolate->NewString("permission");

    result_t hr;
    hr = fillAction(account, name, args, act);
    if (hr < 0)
        return hr;

    int32_t len = authorization->Length();
    int32_t i;
    v8::Local<v8::Context> context = js_module_interface::g_isolate->context();
    act.authorization.resize(len);
    for (i = 0; i < len; i++) {
        JSValue v = authorization->Get(context, i);
        if (!v->IsObject())
            return CALL_E_INVALIDARG;
        v8::Local<v8::Object> o = v8::Local<v8::Object>::Cast(v);

        auto auth_actor = act.authorization[i].actor.to_uint64_t();
        hr = string_to_name(js_module_interface::g_isolate, o->Get(context, actor), auth_actor);
        act.authorization[i].actor = account_name(auth_actor);
        if (hr < 0)
            return hr;

        auto auth_perm = act.authorization[i].permission.to_uint64_t();
        hr = string_to_name(js_module_interface::g_isolate, o->Get(context, permission), auth_perm);
        act.authorization[i].permission = permission_name(auth_perm);
        if (hr < 0)
            return hr;
    }

    return 0;
}

result_t trans_base::send_inline(exlib::string account, exlib::string name, v8::Local<v8::Object> args,
    v8::Local<v8::Array> authorization)
{
    if (g_now_context) {
        result_t hr;
        action act;

        hr = fillAction(account, name, args, authorization, act);
        if (hr < 0)
            return hr;

        try {
            g_now_context->execute_inline(std::move(act));
        } catch (const chain_exception e) {
            return CHECK_ERROR(Runtime::setError(e.to_detail_string()));
        } catch (const std::exception& e) {
            return CHECK_ERROR(Runtime::setError(e.what()));
        } catch (...) {
            return CHECK_ERROR(Runtime::setError("unknown exception"));
        }
    }

    return 0;
}

result_t trans_base::send_context_free_inline(exlib::string account, exlib::string name,
    v8::Local<v8::Object> args)
{
    if (g_now_context) {
        result_t hr;
        action act;

        hr = fillAction(account, name, args, act);
        if (hr < 0)
            return hr;

        try {
            g_now_context->execute_context_free_inline(std::move(act));
        } catch (const chain_exception e) {
            return CHECK_ERROR(Runtime::setError(e.to_detail_string()));
        } catch (const std::exception& e) {
            return CHECK_ERROR(Runtime::setError(e.what()));
        } catch (...) {
            return CHECK_ERROR(Runtime::setError("unknown exception"));
        }
    }

    return 0;
}
} // namespace fibjs
