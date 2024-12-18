#include "config.hpp"

#include "fibjs.h"
#include "object.h"
#include "../include/js_module.h"
#include "js_value.h"
#include "ifs/action.h"
#include <eosio/chain/transaction_context.hpp>

using namespace eosio::chain;

namespace fibjs {

DECLARE_MODULE(action);

result_t action_base::get_name(exlib::string& retVal)
{
    if (g_now_context)
        retVal = g_now_context->get_action().name.to_string();
    return 0;
}

result_t action_base::get_account(exlib::string& retVal)
{
    if (g_now_context)
        retVal = g_now_context->get_action().account.to_string();
    return 0;
}

result_t action_base::get_receiver(exlib::string& retVal)
{
    if (g_now_context)
        retVal = g_now_context->get_receiver().to_string();
    return 0;
}

result_t action_base::get_publication_time(double& retVal)
{
    if (g_now_context)
        retVal = static_cast<double>(g_now_context->trx_context.published.time_since_epoch().count());
    return 0;
}

result_t action_base::get_authorization(v8::Local<v8::Array>& retVal)
{
    JSArray arr = v8::Array::New(js_module_interface::g_isolate->m_isolate);

    if (g_now_context) {
        v8::Local<v8::Context> context = js_module_interface::g_isolate->context();
        v8::Local<v8::String> actor = js_module_interface::g_isolate->NewString("actor");
        v8::Local<v8::String> permission = js_module_interface::g_isolate->NewString("permission");

        int32_t sz = (int32_t)g_now_context->get_action().authorization.size();
        int32_t i;

        for (i = 0; i < sz; i++) {
            const permission_level& p = g_now_context->get_action().authorization[i];
            v8::Local<v8::Object> obj = v8::Object::New(js_module_interface::g_isolate->m_isolate);

            obj->Set(context, actor, js_module_interface::g_isolate->NewString(p.actor.to_string())).IsJust();
            obj->Set(context, permission, js_module_interface::g_isolate->NewString(p.permission.to_string())).IsJust();

            arr->Set(context, i, obj).IsJust();
        }
    }

    retVal = arr;

    return 0;
}

result_t action_base::get_id(exlib::string& retVal)
{
    if (g_now_context)
        retVal = g_now_context->trx_context.packed_trx.id().str();
    return 0;
}

result_t action_base::is_account(exlib::string name, bool& retVal)
{
    if (g_now_context) {
        uint64_t id;
        result_t hr;

        hr = string_to_name(name, id);
        if (hr < 0)
            return hr;

        retVal = g_now_context->is_account(eosio::chain::name(id));
    }

    return 0;
}

result_t action_base::has_recipient(exlib::string name, bool& retVal)
{
    if (g_now_context) {
        uint64_t id;
        result_t hr;

        hr = string_to_name(name, id);
        if (hr < 0)
            return hr;

        retVal = g_now_context->has_recipient(eosio::chain::name(id));
    }

    return 0;
}

result_t action_base::require_recipient(exlib::string name)
{
    if (g_now_context) {
        uint64_t id;
        result_t hr;

        hr = string_to_name(name, id);
        if (hr < 0)
            return hr;

        try {
            g_now_context->require_recipient(eosio::chain::name(id));
        } catch (const chain_exception& e) {
            return CHECK_ERROR(Runtime::setError(e.to_detail_string()));
        } catch (const std::exception& e) {
            return CHECK_ERROR(Runtime::setError(e.what()));
        } catch (...) {
            return CHECK_ERROR(Runtime::setError("unknown exception"));
        }
    }

    return 0;
}

result_t action_base::has_auth(exlib::string name, bool& retVal)
{
    if (g_now_context) {
        uint64_t id;
        result_t hr;

        hr = string_to_name(name, id);
        if (hr < 0)
            return hr;

        retVal = g_now_context->has_authorization(eosio::chain::name(id));
    }

    return 0;
}

result_t action_base::require_auth(exlib::string name, exlib::string permission)
{
    if (g_now_context) {
        uint64_t id, pms;
        result_t hr;

        hr = string_to_name(name, id);
        if (hr < 0)
            return hr;

        if (!permission.empty()) {
            hr = string_to_name(permission, pms);
            if (hr < 0)
                return hr;
        }

        try {
            if (permission.empty())
                g_now_context->require_authorization(eosio::chain::name(id));
            else
                g_now_context->require_authorization(eosio::chain::name(id), eosio::chain::name(pms));
        } catch (const chain_exception& e) {
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