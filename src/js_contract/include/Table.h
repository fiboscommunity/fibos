#pragma once

#include "ifs/Table.h"
#include <eosio/chain/contract_table_objects.hpp>
#include <eosio/chain/abi_def.hpp>
#include <eosio/chain/abi_serializer.hpp>
#include <vector>

namespace fibjs {

class Index;
class Table : public Table_base {
public:
    Table(const eosio::chain::table_def& def,
        uint64_t code, uint64_t scope)
        : m_code(code)
        , m_scope(scope)
        , m_def(def)
    {
    }

public:
    // Table_base
    virtual result_t get_name(exlib::string& retVal);
    virtual result_t get_code(exlib::string& retVal);
    virtual result_t get_scope(exlib::string& retVal);
    virtual result_t emplace(exlib::string payer, v8::Local<v8::Object> val);
    virtual result_t find(v8::Local<v8::Value> id, obj_ptr<DBIterator_base>& retVal);
    virtual result_t get_primary_key(v8::Local<v8::Value>& retVal);
    virtual result_t begin(obj_ptr<DBIterator_base>& retVal);
    virtual result_t end(obj_ptr<DBIterator_base>& retVal);
    virtual result_t lowerbound(v8::Local<v8::Value> id, obj_ptr<DBIterator_base>& retVal);
    virtual result_t upperbound(v8::Local<v8::Value> id, obj_ptr<DBIterator_base>& retVal);
    virtual result_t get_indexes(v8::Local<v8::Object>& retVal);

public:
    static v8::Local<v8::Object> tables();
    static result_t get_index(JSValue v, uint64_t& id);
    result_t get(int64_t itr, uint64_t id, int32_t idx_itr, Index* idx, obj_ptr<DBIterator_base>& retVal);
    result_t get(uint64_t id, int32_t idx_itr, Index* idx, obj_ptr<DBIterator_base>& retVal);

    result_t erase(JSValue id);
    result_t modify(JSValue id, exlib::string payer, v8::Local<v8::Object> val);

    void store_index(uint64_t pyr, uint64_t id, v8::Local<v8::Object> val);
    void update_index(uint64_t payer, uint64_t id, JSValue val);
    void remove_index(uint64_t id);

    result_t set_index(JSValue name, JSValue opt);

public:
    uint64_t m_code;
    uint64_t m_scope;
    const eosio::chain::table_def& m_def;
};
}
