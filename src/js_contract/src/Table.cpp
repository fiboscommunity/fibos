#include "config.hpp"

#include <eosio/chain/controller.hpp>
#include <eosio/chain/transaction.hpp>
#include <eosio/chain/contract_table_objects.hpp>
#include <fc/utility.hpp>
#include <sstream>
#include <algorithm>
#include <set>

#define private public
#include <eosio/chain/apply_context.hpp>
#undef private

#include "object.h"
#include "../include/Table.h"
#include "../include/Index.h"
#include "../include/DBIterator.h"
#include "../include/js_module.h"
#include "js_value.h"
#include "v8_api.h"

namespace eosio {
namespace chain {
    void db_get_pid(apply_context* context, int iterator, uint64_t& primary)
    {
        const auto& obj = context->keyval_cache.get(iterator);
        const auto& idx = context->db.get_index<key_value_index, by_scope_primary>();

        auto itr = idx.iterator_to(obj);
        primary = itr->primary_key;
    }
}
}

using namespace eosio::chain;

namespace fibjs {

class TableFactory : public object_base {
public:
    TableFactory(const exlib::string name, const table_def& def)
        : m_def(def)
    {
    }

    static void get_table(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        result_t hr;
        int32_t argc = args.Length();
        uint64_t code, scope;
        v8::Local<v8::Context> context = js_module_interface::g_isolate->context();

        if (argc < 2) {
            ThrowResult(CALL_E_PARAMNOTOPTIONAL);
            return;
        }

        hr = string_to_name(js_module_interface::g_isolate, args[0], code);
        if (hr < 0) {
            ThrowResult(hr);
            return;
        }

        hr = string_to_name(js_module_interface::g_isolate, args[1], scope);
        if (hr < 0) {
            ThrowResult(hr);
            return;
        }

        obj_ptr<TableFactory> tf = (TableFactory*)object_base::getInstance(args.Data()->ToObject(context).FromMaybe(v8::Local<v8::Object>()));
        obj_ptr<Table> tab = new Table(tf->m_def, code, scope);

        if (tf->m_def.key_types.size() == 0 || tf->m_def.key_names.size() == 0) {
            ThrowResult(Runtime::setError("key types or key names should not be empty."));
            return;
        }

        tab->SetPrivate("indexes", v8::Object::New(js_module_interface::g_isolate->m_isolate));
        tab->SetPrivate("indexes_array", v8::Array::New(js_module_interface::g_isolate->m_isolate));

        if (argc > 2) {
            if (!args[2]->IsObject()) {
                ThrowResult(CALL_E_BADVARTYPE);
                return;
            }

            v8::Local<v8::Object> o = v8::Local<v8::Object>::Cast(args[2]);
            JSArray ks = o->GetPropertyNames(context);
            int32_t len = ks->Length();

            if (len > 16) {
                ThrowResult(Runtime::setError("multi_index only supports a maximum of 16 secondary indices."));
                return;
            }

            for (int32_t i = 0; i < len; i++) {
                JSValue k = ks->Get(context, i);
                hr = tab->set_index(k, o->Get(context, k).FromMaybe(v8::Local<v8::Value>()));
                if (hr < 0) {
                    ThrowResult(hr);
                    return;
                }
            }
        }

        args.GetReturnValue().Set(tab->wrap());
    }

public:
    const table_def& m_def;
};

v8::Intercepted get_table(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
    if (js_module_interface::g_module != NULL) {
        v8::String::Utf8Value str(js_module_interface::g_isolate->m_isolate, property);

        for (int32_t i = 0; i < (int32_t)js_module_interface::g_module->m_abi.tables.size(); i++) {
            exlib::string name = js_module_interface::g_module->m_abi.tables[i].name.to_string();

            if (name == ToCString(str)) {
                obj_ptr<TableFactory> tf = new TableFactory(name, js_module_interface::g_module->m_abi.tables[i]);

                info.GetReturnValue().Set(js_module_interface::g_isolate->NewFunction(name.c_str(), TableFactory::get_table, tf->wrap()));
                return v8::Intercepted::kYes;;
            }
        }
    }

    return v8::Intercepted::kNo;
}

v8::Intercepted set_table(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info)
{
    v8::String::Utf8Value str(js_module_interface::g_isolate->m_isolate, property);

    if (qstrcmp(*str, "Table") && qstrcmp(*str, "DBIterator"))
        return v8::Intercepted::kYes;
    return v8::Intercepted::kNo;
}

void enum_table(const v8::PropertyCallbackInfo<v8::Array>& info)
{
    JSArray a = v8::Array::New(js_module_interface::g_isolate->m_isolate);

    if (js_module_interface::g_module != NULL)
        for (int32_t i = 0; i < (int32_t)js_module_interface::g_module->m_abi.tables.size(); i++)
            a->Set(js_module_interface::g_isolate->context(), i,
                 js_module_interface::g_isolate->NewString(js_module_interface::g_module->m_abi.tables[i].name.to_string()))
                .IsJust();

    info.GetReturnValue().Set(a);
}

// var tab1 = db.tab1(code, scope);
v8::Local<v8::Object> Table::tables()
{
    v8::Local<v8::FunctionTemplate> _class = v8::FunctionTemplate::New(js_module_interface::g_isolate->m_isolate);
    v8::Local<v8::Context> context = js_module_interface::g_isolate->context();

    _class->SetClassName(js_module_interface::g_isolate->NewString("db"));
    _class->InstanceTemplate()->SetHandler(v8::NamedPropertyHandlerConfiguration(get_table, set_table, nullptr, nullptr, enum_table, v8::Local<v8::Value>(),
        v8::PropertyHandlerFlags::kOnlyInterceptStrings));

    v8::Local<v8::Function> _function = _class->GetFunction(context).FromMaybe(v8::Local<v8::Function>());
    v8::Local<v8::Object> db = _function->NewInstance(context).FromMaybe(v8::Local<v8::Object>());

    db->DefineOwnProperty(context, js_module_interface::g_isolate->NewString("Table"),
          Table_base::class_info().getModule(js_module_interface::g_isolate), v8::DontEnum)
        .IsJust();
    db->DefineOwnProperty(context, js_module_interface::g_isolate->NewString("DBIterator"),
          DBIterator_base::class_info().getModule(js_module_interface::g_isolate), v8::DontEnum)
        .IsJust();

    return db;
}

result_t Table::get_name(exlib::string& retVal)
{
    retVal = m_def.name.to_string();
    return 0;
}

result_t Table::get_code(exlib::string& retVal)
{
    name v = name(m_code);
    retVal = v.to_string();
    return 0;
}

result_t Table::get_scope(exlib::string& retVal)
{
    name v = name(m_scope);
    retVal = v.to_string();
    return 0;
}

result_t Table::get_index(JSValue v, uint64_t& id)
{
    v8::Local<v8::Context> context = js_module_interface::g_isolate->context();
    if (v->IsUndefined())
        return CHECK_ERROR(Runtime::setError("missing primary key."));

    if (v->IsNumber() || v->IsNumberObject()) {
        id = v->NumberValue(context).FromMaybe(0);
    } else if (v->IsBigInt() || v->IsBigIntObject()) {
        id = v->ToBigInt(context).FromMaybe(v8::Local<v8::BigInt>())->Uint64Value();
    } else
        return string_to_name(js_module_interface::g_isolate, v, id);

    return 0;
}

void Table::remove_index(uint64_t id)
{
    JSArray indexes_array = JSArray::Cast(GetPrivate("indexes_array"));
    int32_t len = indexes_array->Length();

    for (int32_t i = 0; i < len; i++) {
        obj_ptr<Index> idx = (Index*)Table_base::getInstance(indexes_array->Get(js_module_interface::g_isolate->context(), i).FromMaybe(v8::Local<v8::Value>()));
        idx->remove(id);
    }
}

void Table::store_index(uint64_t pyr, uint64_t id, v8::Local<v8::Object> val)
{
    JSArray indexes_array = JSArray::Cast(GetPrivate("indexes_array"));
    int32_t len = indexes_array->Length();

    for (int32_t i = 0; i < len; i++) {
        obj_ptr<Index> idx = (Index*)Table_base::getInstance(indexes_array->Get(js_module_interface::g_isolate->context(), i).FromMaybe(v8::Local<v8::Value>()));
        idx->store(pyr, id, val);
    }
}

void Table::update_index(uint64_t pyr, uint64_t id, JSValue val)
{
    JSArray indexes_array = JSArray::Cast(GetPrivate("indexes_array"));
    int32_t len = indexes_array->Length();

    for (int32_t i = 0; i < len; i++) {
        obj_ptr<Index> idx = (Index*)Table_base::getInstance(indexes_array->Get(js_module_interface::g_isolate->context(), i).FromMaybe(v8::Local<v8::Value>()));
        idx->update(pyr, id, val);
    }
}

result_t Table::emplace(exlib::string payer, v8::Local<v8::Object> val)
{
    if (g_now_context) {
        result_t hr;
        uint64_t pyr;
        uint64_t id;

        hr = string_to_name(payer, pyr);
        if (hr < 0)
            return hr;

        hr = get_index(val->Get(js_module_interface::g_isolate->context(), holder()->NewString(m_def.key_names[0])), id);
        if (hr < 0)
            return hr;

        fc::variant v = fc_value(js_module_interface::g_isolate, val);
        bytes data;

        try {
            data = js_module_interface::g_module->m_abis.variant_to_binary(m_def.type, v, abi_serializer::create_yield_function(max_serialization_time));

            g_now_context->db_store_i64(name(m_scope), m_def.name, account_name(pyr), id, data.data(), data.size());
            store_index(pyr, id, val);
        } catch (const chain_exception& e) {
            return CHECK_ERROR(Runtime::setError(e.to_detail_string()));
        } catch (const fc::exception& e) {
            return CHECK_ERROR(Runtime::setError(e.to_detail_string()));
        } catch (const std::exception& e) {
            return CHECK_ERROR(Runtime::setError(e.what()));
        } catch (...) {
            return CHECK_ERROR(Runtime::setError("unknown exception"));
        }
    }

    return 0;
}

result_t Table::get_primary_key(v8::Local<v8::Value>& retVal)
{
    result_t hr;
    int32_t enditr;
    uint64_t endpk;
    uint64_t _next_primary_key;

    try {
        enditr = g_now_context->db_end_i64(name(m_code), name(m_scope), m_def.name);
        if (enditr == -1) {
            _next_primary_key = 0;
        } else {
            hr = g_now_context->db_previous_i64(enditr, endpk);
            if (hr == -1) {
                _next_primary_key = 0;
            } else {
                _next_primary_key = endpk;
                _next_primary_key++;
            }
        }
    } catch (const chain_exception& e) {
        return CHECK_ERROR(Runtime::setError(e.to_detail_string()));
    } catch (const std::exception& e) {
        return CHECK_ERROR(Runtime::setError(e.what()));
    } catch (...) {
        return CHECK_ERROR(Runtime::setError("unknown exception"));
    }

    retVal = v8::BigInt::NewFromUnsigned(js_module_interface::g_isolate->m_isolate, _next_primary_key);
    return 0;
}

result_t Table::begin(obj_ptr<DBIterator_base>& retVal)
{
    int32_t itr;
    result_t hr;
    uint64_t id;

    if (g_now_context) {
        hr = string_to_name(m_def.key_names[0], id);
        if (hr < 0)
            return hr;

        try {
            itr = g_now_context->db_lowerbound_i64(name(m_code), name(m_scope), m_def.name, 0);
            if (itr < 0)
                itr = -2;
        } catch (const chain_exception& e) {
            return CHECK_ERROR(Runtime::setError(e.to_detail_string()));
        } catch (const std::exception& e) {
            return CHECK_ERROR(Runtime::setError(e.what()));
        } catch (...) {
            return CHECK_ERROR(Runtime::setError("unknown exception"));
        }

        return get(itr, id, -1, NULL, retVal);
    }

    return CALL_RETURN_NULL;
}

result_t Table::end(obj_ptr<DBIterator_base>& retVal)
{
    int32_t itr;
    result_t hr;
    uint64_t id;

    if (g_now_context) {
        hr = string_to_name(m_def.key_names[0], id);
        if (hr < 0)
            return hr;

        try {
            itr = g_now_context->db_end_i64(name(m_code), name(m_scope), m_def.name);
            if (itr < 0)
                itr = -2;
        } catch (const chain_exception& e) {
            return CHECK_ERROR(Runtime::setError(e.to_detail_string()));
        } catch (const std::exception& e) {
            return CHECK_ERROR(Runtime::setError(e.what()));
        } catch (...) {
            return CHECK_ERROR(Runtime::setError("unknown exception"));
        }

        return get(itr, id, -1, NULL, retVal);
    }

    return CALL_RETURN_NULL;
}

result_t Table::get(int64_t itr, uint64_t id, int32_t idx_itr, Index* idx, obj_ptr<DBIterator_base>& retVal)
{
    obj_ptr<DBIterator> _itr;
    result_t hr;

    try {
        _itr = new DBIterator(this);
        if (itr > -1)
            db_get_pid(g_now_context, itr, id);

        hr = _itr->load(itr, id, idx_itr, idx);
    } catch (const chain_exception& e) {
        return CHECK_ERROR(Runtime::setError(e.to_detail_string()));
    } catch (const std::exception& e) {
        return CHECK_ERROR(Runtime::setError(e.what()));
    } catch (const fc::exception& e) {
        return CHECK_ERROR(Runtime::setError(e.to_detail_string()));
    } catch (...) {
        return CHECK_ERROR(Runtime::setError("unknown exception"));
    }
    if (hr < 0)
        return hr;

    retVal = _itr;

    return 0;
}

result_t Table::get(uint64_t id, int32_t idx_itr, Index* idx, obj_ptr<DBIterator_base>& retVal)
{
    int32_t itr;

    try {
        itr = g_now_context->db_find_i64(name(m_code), name(m_scope), m_def.name, id);
    } catch (const chain_exception& e) {
        return CHECK_ERROR(Runtime::setError(e.to_detail_string()));
    } catch (const std::exception& e) {
        return CHECK_ERROR(Runtime::setError(e.what()));
    } catch (...) {
        return CHECK_ERROR(Runtime::setError("unknown exception"));
    }

    return get(itr, id, idx_itr, idx, retVal);
}

result_t Table::find(v8::Local<v8::Value> id, obj_ptr<DBIterator_base>& retVal)
{
    result_t hr;
    uint64_t _id;

    if (g_now_context) {
        hr = get_index(id, _id);
        if (hr < 0)
            return hr;

        return get(_id, -1, NULL, retVal);
    }

    return CALL_RETURN_NULL;
}

result_t Table::lowerbound(v8::Local<v8::Value> id, obj_ptr<DBIterator_base>& retVal)
{
    int32_t itr;
    result_t hr;
    uint64_t _id;

    if (g_now_context) {
        hr = get_index(id, _id);
        if (hr < 0)
            return hr;

        try {
            itr = g_now_context->db_lowerbound_i64(name(m_code), name(m_scope), m_def.name, _id);
            if (itr < 0)
                itr = -2;
        } catch (const chain_exception& e) {
            return CHECK_ERROR(Runtime::setError(e.to_detail_string()));
        } catch (const std::exception& e) {
            return CHECK_ERROR(Runtime::setError(e.what()));
        } catch (...) {
            return CHECK_ERROR(Runtime::setError("unknown exception"));
        }

        return get(itr, _id, -1, NULL, retVal);
    }

    return CALL_RETURN_NULL;
}

result_t Table::upperbound(v8::Local<v8::Value> id, obj_ptr<DBIterator_base>& retVal)
{
    int32_t itr;
    result_t hr;
    uint64_t _id;

    if (g_now_context) {
        hr = get_index(id, _id);
        if (hr < 0)
            return hr;

        try {
            itr = g_now_context->db_upperbound_i64(name(m_code), name(m_scope), m_def.name, _id);
            if (itr < 0)
                itr = -2;
        } catch (const chain_exception& e) {
            return CHECK_ERROR(Runtime::setError(e.to_detail_string()));
        } catch (const std::exception& e) {
            return CHECK_ERROR(Runtime::setError(e.what()));
        } catch (...) {
            return CHECK_ERROR(Runtime::setError("unknown exception"));
        }

        return get(itr, _id, -1, NULL, retVal);
    }

    return CALL_RETURN_NULL;
}

result_t Table::set_index(JSValue name, JSValue opt)
{
    result_t hr;
    uint64_t idx_name;
    v8::Local<v8::Context> context = js_module_interface::g_isolate->context();

    hr = string_to_name(js_module_interface::g_isolate, name, idx_name);
    if (hr < 0)
        return hr;

    if ((idx_name & 0x000000000000000FULL) != 0)
        return Runtime::setError("multi_index does not support table names with a length greater than 12.");

    v8::Local<v8::Array> a;
    hr = GetArgumentValue(js_module_interface::g_isolate, opt, a, true);
    if (hr < 0)
        return hr;

    if (a->Length() != 2)
        return Runtime::setError("multi_index requires two parameters.");

    int32_t sz;
    hr = GetArgumentValue(js_module_interface::g_isolate, a->Get(context, 0).FromMaybe(v8::Local<v8::Value>()), sz, true);
    if (hr < 0)
        return hr;

    if (sz <= 0 || sz > 256)
        return Runtime::setError("the size of the index key is out of range.");

    if (sz % 64 != 0)
        return Runtime::setError("the size of the index key must be a multiple of 64.");

    v8::Local<v8::Function> func;
    hr = GetArgumentValue(js_module_interface::g_isolate, a->Get(context, 1).FromMaybe(v8::Local<v8::Value>()), func, true);
    if (hr < 0)
        return hr;

    JSArray indexes_array = JSArray::Cast(GetPrivate("indexes_array"));
    int32_t len = indexes_array->Length();
    obj_ptr<Index> idx;

    switch (sz) {
    case 64:
        idx = new Index64(this, len, func);
        break;
    case 128:
        idx = new Index128(this, len, func);
        break;
    case 192:
        idx = new Index192(this, len, func);
        break;
    case 256:
        idx = new Index256(this, len, func);
        break;
    }

    indexes_array->Set(context, len, idx->wrap()).IsJust();

    v8::Local<v8::Object> indexes;
    get_indexes(indexes);

    eosio::chain::name v = eosio::chain::name(idx_name);
    indexes->Set(context, js_module_interface::g_isolate->NewString(v.to_string()), idx->wrap()).IsJust();
    return 0;
}

result_t Table::get_indexes(v8::Local<v8::Object>& retVal)
{
    retVal = v8::Local<v8::Object>::Cast(GetPrivate("indexes"));
    return 0;
}
}
