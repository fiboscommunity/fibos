#pragma once

#include "ifs/DBIterator.h"
#include "Table.h"
#include "Index.h"

namespace fibjs {

class DBIterator : public DBIterator_base {
public:
    DBIterator(Table* table)
        : m_def(table->m_def)
        , m_code(table->m_code)
        , m_scope(table->m_scope)
        , m_itr(0)
    {
        SetPrivate("table", table->wrap());
    }

public:
    // DBIterator_base
    virtual result_t is_begin(bool& retVal);
    virtual result_t is_end(bool& retVal);
    virtual result_t next(obj_ptr<DBIterator_base>& retVal);
    virtual result_t previous(obj_ptr<DBIterator_base>& retVal);
    virtual result_t remove();
    virtual result_t update(exlib::string payer);
    virtual result_t get_data(v8::Local<v8::Value>& retVal);

public:
    result_t load(int32_t itr, uint64_t id, int32_t idx_itr, Index* idx);

public:
    const eosio::chain::table_def& m_def;
    uint64_t m_code;
    uint64_t m_scope;
    uint64_t m_table;
    uint64_t m_id;
    int32_t m_itr;
    int32_t m_idx_itr;
};
}
