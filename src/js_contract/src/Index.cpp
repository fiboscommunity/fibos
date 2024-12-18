#include "config.hpp"

#include "object.h"
#include "../include/Table.h"
#include "../include/Index.h"
#include "../include/DBIterator.h"
#include "../include/js_module.h"
#include "js_value.h"
#include "v8_api.h"

using namespace eosio::chain;

namespace fibjs {

void Index::build_key(JSValue val, key_t& ks)
{
    TryCatch try_catch;
    v8::Local<v8::Context> context = js_module_interface::g_isolate->context();

    v8::Local<v8::Function> func = v8::Local<v8::Function>::Cast(GetPrivate("func"));
    JSValue v = func->Call(context, val, 1, &val);
    EOS_ASSERT(!v.IsEmpty(), js_execution_error, GetException(try_catch, CALL_E_JAVASCRIPT, false, true));
    EOS_ASSERT(v->IsArray(), js_execution_error, "the index key must be an array.");

    JSArray k = JSArray::Cast(v);
    EOS_ASSERT(k->Length() == m_sz, js_execution_error, "the index key size must be ${sz}.", ("sz", 54 * m_sz));

    for (int32_t i = 0; i < m_sz; i++) {
        result_t hr = Table::get_index(k->Get(context, i), ks[i]);
        EOS_ASSERT(hr >= 0, js_execution_error, Runtime::errMessage());
    }
}

result_t Index::get_name(exlib::string& retVal)
{
    retVal = m_def.name.to_string();
    return 0;
}

result_t Index::get_code(exlib::string& retVal)
{
    eosio::chain::name v = eosio::chain::name(m_code);
    retVal = v.to_string();
    return 0;
}

result_t Index::get_scope(exlib::string& retVal)
{
    eosio::chain::name v = eosio::chain::name(m_scope);
    retVal = v.to_string();
    return 0;
}

result_t Index::emplace(exlib::string payer, v8::Local<v8::Object> val)
{
    return CALL_E_INVALID_CALL;
}

result_t Index::get_primary_key(v8::Local<v8::Value>& retVal)
{
    return CALL_E_INVALID_CALL;
}

result_t Index::begin(obj_ptr<DBIterator_base>& retVal)
{
    return CALL_E_INVALID_CALL;
}

result_t Index::end(obj_ptr<DBIterator_base>& retVal)
{
    return CALL_E_INVALID_CALL;
}

result_t Index::find(v8::Local<v8::Value> id, obj_ptr<DBIterator_base>& retVal)
{
    key_t ks = { 0 };
    uint64_t pid;
    int32_t itr;

    try {
        build_key(id, ks);
        itr = find(ks, pid);
        if (itr < 0)
            pid = -1;

        obj_ptr<Table> table = (Table*)Table_base::getInstance(GetPrivate("table"));
        return table->get(pid, itr, this, retVal);
    } catch (const chain_exception& e) {
        return CHECK_ERROR(Runtime::setError(e.to_detail_string()));
    } catch (const std::exception& e) {
        return CHECK_ERROR(Runtime::setError(e.what()));
    } catch (...) {
        return CHECK_ERROR(Runtime::setError("unknown exception"));
    }

    return CALL_RETURN_NULL;
}

result_t Index::lowerbound(v8::Local<v8::Value> id, obj_ptr<DBIterator_base>& retVal)
{
    key_t ks = { 0 };
    uint64_t pid;
    int32_t itr;

    try {
        build_key(id, ks);
        itr = lowerbound(ks, pid);
        if (itr < 0)
            pid = -1;

        obj_ptr<Table> table = (Table*)Table_base::getInstance(GetPrivate("table"));
        return table->get(pid, itr, this, retVal);
    } catch (const chain_exception& e) {
        return CHECK_ERROR(Runtime::setError(e.to_detail_string()));
    } catch (const std::exception& e) {
        return CHECK_ERROR(Runtime::setError(e.what()));
    } catch (...) {
        return CHECK_ERROR(Runtime::setError("unknown exception"));
    }

    return CALL_RETURN_NULL;
}

result_t Index::upperbound(v8::Local<v8::Value> id, obj_ptr<DBIterator_base>& retVal)
{
    key_t ks = { 0 };
    uint64_t pid;
    int32_t itr;

    try {
        build_key(id, ks);
        itr = upperbound(ks, pid);
        if (itr < 0)
            pid = -1;

        obj_ptr<Table> table = (Table*)Table_base::getInstance(GetPrivate("table"));
        return table->get(pid, itr, this, retVal);
    } catch (const chain_exception& e) {
        return CHECK_ERROR(Runtime::setError(e.to_detail_string()));
    } catch (const std::exception& e) {
        return CHECK_ERROR(Runtime::setError(e.what()));
    } catch (...) {
        return CHECK_ERROR(Runtime::setError("unknown exception"));
    }

    return CALL_RETURN_NULL;
}

result_t Index::remove(DBIterator_base* iter)
{
    return CALL_E_INVALID_CALL;
}

result_t Index::update(exlib::string payer, DBIterator_base* iter)
{
    return CALL_E_INVALID_CALL;
}

result_t Index::get_indexes(v8::Local<v8::Object>& retVal)
{
    return CALL_E_INVALID_CALL;
}

void Index::store(uint64_t pyr, uint64_t id, JSValue val)
{
    key_t ks = { 0 };
    build_key(val, ks);
    store(pyr, id, ks);
}

void Index::update(uint64_t pyr, uint64_t id, JSValue val)
{
    key_t ks = { 0 };
    build_key(val, ks);
    update(pyr, id, ks);
}

// Index64
void Index64::store(uint64_t pyr, uint64_t id, key_t& k)
{
    g_now_context->idx64.store(m_scope, name(), account_name(pyr), id, k[0]);
}

void Index64::update(uint64_t pyr, uint64_t id, key_t& k)
{
    key_t k1 = { 0 };
    int32_t itr = g_now_context->idx64.find_primary(m_code, m_scope, name(), k1[0], id);
    if (k[0] != k1[0])
        g_now_context->idx64.update(itr, account_name(pyr), k[0]);
}

int32_t Index64::find(const key_t& k, uint64_t& id)
{
    return g_now_context->idx64.find_secondary(m_code, m_scope, name(), k[0], id);
}

void Index64::remove(uint64_t id)
{
    key_t k;
    g_now_context->idx64.remove(g_now_context->idx64.find_primary(m_code, m_scope, name(), k[0], id));
}

int32_t Index64::previous(int32_t itr, uint64_t& id)
{
    return g_now_context->idx64.previous_secondary(itr, id);
}

int32_t Index64::next(int32_t itr, uint64_t& id)
{
    return g_now_context->idx64.next_secondary(itr, id);
}

int32_t Index64::lowerbound(key_t& k, uint64_t& id)
{
    return g_now_context->idx64.lowerbound_secondary(m_code, m_scope, name(), k[0], id);
}

int32_t Index64::upperbound(key_t& k, uint64_t& id)
{
    return g_now_context->idx64.upperbound_secondary(m_code, m_scope, name(), k[0], id);
}

// Index128
void Index128::store(uint64_t pyr, uint64_t id, key_t& k)
{
    g_now_context->idx128.store(m_scope, name(), account_name(pyr), id, *(eosio::chain::uint128_t*)&k);
}

void Index128::update(uint64_t pyr, uint64_t id, key_t& k)
{
    key_t k1 = { 0 };
    int32_t itr = g_now_context->idx128.find_primary(m_code, m_scope, name(), *(eosio::chain::uint128_t*)&k1, id);
    if (*(eosio::chain::uint128_t*)&k != *(eosio::chain::uint128_t*)&k1)
        g_now_context->idx128.update(itr, account_name(pyr), *(eosio::chain::uint128_t*)&k);
}

int32_t Index128::find(const key_t& k, uint64_t& id)
{
    return g_now_context->idx128.find_secondary(m_code, m_scope, name(), *(eosio::chain::uint128_t*)&k, id);
}

void Index128::remove(uint64_t id)
{
    key_t k;
    g_now_context->idx128.remove(g_now_context->idx128.find_primary(m_code, m_scope, name(), *(eosio::chain::uint128_t*)&k, id));
}

int32_t Index128::previous(int32_t itr, uint64_t& id)
{
    return g_now_context->idx128.previous_secondary(itr, id);
}

int32_t Index128::next(int32_t itr, uint64_t& id)
{
    return g_now_context->idx128.next_secondary(itr, id);
}

int32_t Index128::lowerbound(key_t& k, uint64_t& id)
{
    return g_now_context->idx128.lowerbound_secondary(m_code, m_scope, name(), *(eosio::chain::uint128_t*)&k, id);
}

int32_t Index128::upperbound(key_t& k, uint64_t& id)
{
    return g_now_context->idx128.upperbound_secondary(m_code, m_scope, name(), *(eosio::chain::uint128_t*)&k, id);
}

// Index256
void Index256::store(uint64_t pyr, uint64_t id, key_t& k)
{
    g_now_context->idx256.store(m_scope, name(), account_name(pyr), id, (eosio::chain::uint128_t*)&k);
}

void Index256::update(uint64_t pyr, uint64_t id, key_t& k)
{
    key_t k1 = { 0 };
    int32_t itr = g_now_context->idx256.find_primary(m_code, m_scope, name(), (eosio::chain::uint128_t*)&k1, id);
    if (memcmp(&k, &k1, sizeof(k)))
        g_now_context->idx256.update(itr, account_name(pyr), (eosio::chain::uint128_t*)&k);
}

int32_t Index256::find(const key_t& k, uint64_t& id)
{
    return g_now_context->idx256.find_secondary(m_code, m_scope, name(), (eosio::chain::uint128_t*)&k, id);
}

void Index256::remove(uint64_t id)
{
    key_t k;
    g_now_context->idx256.remove(g_now_context->idx256.find_primary(m_code, m_scope, name(), (eosio::chain::uint128_t*)&k, id));
}

int32_t Index256::previous(int32_t itr, uint64_t& id)
{
    return g_now_context->idx256.previous_secondary(itr, id);
}

int32_t Index256::next(int32_t itr, uint64_t& id)
{
    return g_now_context->idx256.next_secondary(itr, id);
}

int32_t Index256::lowerbound(key_t& k, uint64_t& id)
{
    return g_now_context->idx256.lowerbound_secondary(m_code, m_scope, name(), (eosio::chain::uint128_t*)&k, id);
}

int32_t Index256::upperbound(key_t& k, uint64_t& id)
{
    return g_now_context->idx256.upperbound_secondary(m_code, m_scope, name(), (eosio::chain::uint128_t*)&k, id);
}
}
