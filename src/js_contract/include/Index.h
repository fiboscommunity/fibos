#pragma once

#include "js_module.h"
#include "Table.h"
#include <eosio/chain/contract_table_objects.hpp>
#include <eosio/chain/abi_def.hpp>
#include <eosio/chain/abi_serializer.hpp>

namespace fibjs {

class Index : public Table_base {
public:
    Index(Table* table, int32_t idx, int32_t sz, v8::Local<v8::Function> func)
        : m_code(table->m_code)
        , m_scope(table->m_scope)
        , m_def(table->m_def)
        , m_idx(idx)
        , m_sz(sz)
    {
        SetPrivate("table", table->wrap());
        SetPrivate("func", func);
    }

public:
    // Table_base
    virtual result_t get_name(exlib::string& retVal);
    virtual result_t get_code(exlib::string& retVal);
    virtual result_t get_scope(exlib::string& retVal);
    virtual result_t emplace(exlib::string payer, v8::Local<v8::Object> val);
    virtual result_t get_primary_key(v8::Local<v8::Value>& retVal);
    virtual result_t begin(obj_ptr<DBIterator_base>& retVal);
    virtual result_t end(obj_ptr<DBIterator_base>& retVal);
    virtual result_t find(v8::Local<v8::Value> id, obj_ptr<DBIterator_base>& retVal);
    virtual result_t lowerbound(v8::Local<v8::Value> id, obj_ptr<DBIterator_base>& retVal);
    virtual result_t upperbound(v8::Local<v8::Value> id, obj_ptr<DBIterator_base>& retVal);
    virtual result_t remove(DBIterator_base* iter);
    virtual result_t update(exlib::string payer, DBIterator_base* iter);
    virtual result_t get_indexes(v8::Local<v8::Object>& retVal);

public:
    void store(uint64_t pyr, uint64_t id, JSValue val);
    void update(uint64_t pyr, uint64_t id, JSValue val);
    virtual void remove(uint64_t id) = 0;

    uint64_t name()
    {
        return m_def.name.to_uint64_t() | m_idx;
    }

public:
    // Index api
    typedef uint64_t key_t[4];
    void build_key(JSValue val, key_t& ks);
    virtual void store(uint64_t pyr, uint64_t id, key_t& val) = 0;
    virtual void update(uint64_t pyr, uint64_t id, key_t& val) = 0;
    virtual int32_t find(const key_t& k, uint64_t& id) = 0;
    virtual int32_t previous(int32_t itr, uint64_t& id) = 0;
    virtual int32_t next(int32_t itr, uint64_t& id) = 0;
    virtual int32_t lowerbound(key_t& k, uint64_t& id) = 0;
    virtual int32_t upperbound(key_t& k, uint64_t& id) = 0;

public:
    uint64_t m_code;
    uint64_t m_scope;
    const eosio::chain::table_def& m_def;
    int32_t m_idx;
    int32_t m_sz;
};

class Index64 : public Index {
public:
    Index64(Table* table, int32_t idx, v8::Local<v8::Function> func)
        : Index(table, idx, 1, func)
    {
    }

public:
    // Index api
    virtual void store(uint64_t pyr, uint64_t id, key_t& k);
    virtual void update(uint64_t pyr, uint64_t id, key_t& k);
    virtual int32_t find(const key_t& k, uint64_t& id);
    virtual void remove(uint64_t id);
    virtual int32_t previous(int32_t itr, uint64_t& id);
    virtual int32_t next(int32_t itr, uint64_t& id);
    virtual int32_t lowerbound(key_t& k, uint64_t& id);
    virtual int32_t upperbound(key_t& k, uint64_t& id);
};

class Index128 : public Index {
public:
    Index128(Table* table, int32_t idx, v8::Local<v8::Function> func)
        : Index(table, idx, 2, func)
    {
    }

public:
    // Index api
    virtual void store(uint64_t pyr, uint64_t id, key_t& k);
    virtual void update(uint64_t pyr, uint64_t id, key_t& k);
    virtual int32_t find(const key_t& k, uint64_t& id);
    virtual void remove(uint64_t id);
    virtual int32_t previous(int32_t itr, uint64_t& id);
    virtual int32_t next(int32_t itr, uint64_t& id);
    virtual int32_t lowerbound(key_t& k, uint64_t& id);
    virtual int32_t upperbound(key_t& k, uint64_t& id);
};

class Index256 : public Index {
public:
    Index256(Table* table, int32_t idx, v8::Local<v8::Function> func)
        : Index(table, idx, 4, func)
    {
    }

public:
    // Index api
    virtual void store(uint64_t pyr, uint64_t id, key_t& k);
    virtual void update(uint64_t pyr, uint64_t id, key_t& k);
    virtual int32_t find(const key_t& k, uint64_t& id);
    virtual void remove(uint64_t id);
    virtual int32_t previous(int32_t itr, uint64_t& id);
    virtual int32_t next(int32_t itr, uint64_t& id);
    virtual int32_t lowerbound(key_t& k, uint64_t& id);
    virtual int32_t upperbound(key_t& k, uint64_t& id);
};

class Index192 : public Index256 {
public:
    Index192(Table* table, int32_t idx, v8::Local<v8::Function> func)
        : Index256(table, idx, func)
    {
        m_sz = 3;
    }
};
}
