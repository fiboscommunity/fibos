#include "config.hpp"

#include "../include/DBIterator.h"
#include "../include/js_module.h"
#include "js_value.h"

using namespace eosio::chain;

namespace fibjs {

result_t DBIterator::load(int32_t itr, uint64_t id, int32_t idx_itr, Index* idx)
{
    m_itr = itr;
    m_id = id;
    m_idx_itr = idx_itr;

    if (idx != NULL)
        SetPrivate("index", idx->wrap());

    if (m_itr > -1 && g_now_context) {
        bytes data;

        try {
            int sz = g_now_context->db_get_i64(itr, NULL, 0);
            data.resize(sz);
            g_now_context->db_get_i64(itr, data.data(), data.size());
        } catch (const chain_exception& e) {
            return CHECK_ERROR(Runtime::setError(e.to_detail_string()));
        } catch (const std::exception& e) {
            return CHECK_ERROR(Runtime::setError(e.what()));
        } catch (...) {
            return CHECK_ERROR(Runtime::setError("unknown exception"));
        }

        v8::Local<v8::Value> v = js_value(js_module_interface::g_isolate, js_module_interface::g_module->m_abis.binary_to_variant(m_def.type, data, max_serialization_time));
        SetPrivate("data", v);
    }

    return 0;
}

result_t DBIterator::is_begin(bool& retVal)
{
    if (g_now_context) {
        int32_t itr;
        if (m_idx_itr == -1) {
            itr = g_now_context->db_lowerbound_i64(name(m_code), name(m_scope), m_def.name, 0);

            retVal = m_itr == itr;
        } else {
            Index::key_t ks = { 0 };
            uint64_t id = -1;

            obj_ptr<Index> index = (Index*)Table_base::getInstance(GetPrivate("index"));
            itr = index->lowerbound(ks, id);

            retVal = m_idx_itr == itr;
        }
    } else {
        retVal = false;
    }

    return 0;
}

result_t DBIterator::is_end(bool& retVal)
{
    if (m_idx_itr == -1) {
        retVal = m_itr < 0;
    } else {
        retVal = m_idx_itr < 0;
    }
    return 0;
}

result_t DBIterator::next(obj_ptr<DBIterator_base>& retVal)
{
    if (g_now_context) {
        uint64_t id = -1;
        int32_t itr;

        try {
            obj_ptr<Table> table = (Table*)Table_base::getInstance(GetPrivate("table"));
            if (m_idx_itr == -1) {
                itr = g_now_context->db_next_i64(m_itr, id);
                if (itr < 0)
                    id = -1;

                return table->get(itr, id, -1, NULL, retVal);
            } else {
                obj_ptr<Index> index = (Index*)Table_base::getInstance(GetPrivate("index"));
                itr = index->next(m_idx_itr, id);
                return table->get(id, itr, index, retVal);
            }
        } catch (const chain_exception& e) {
            return CHECK_ERROR(Runtime::setError(e.to_detail_string()));
        } catch (const std::exception& e) {
            return CHECK_ERROR(Runtime::setError(e.what()));
        } catch (...) {
            return CHECK_ERROR(Runtime::setError("unknown exception"));
        }
    }

    return CALL_RETURN_NULL;
}

result_t DBIterator::previous(obj_ptr<DBIterator_base>& retVal)
{
    if (g_now_context) {
        uint64_t id = -1;
        int32_t itr;

        try {
            obj_ptr<Table> table = (Table*)Table_base::getInstance(GetPrivate("table"));
            if (m_idx_itr == -1) {
                itr = g_now_context->db_previous_i64(m_itr, id);
                return table->get(itr, id, -1, NULL, retVal);
            } else {
                obj_ptr<Index> index = (Index*)Table_base::getInstance(GetPrivate("index"));
                itr = index->previous(m_idx_itr, id);
                return table->get(id, itr, index, retVal);
            }
        } catch (const chain_exception& e) {
            return CHECK_ERROR(Runtime::setError(e.to_detail_string()));
        } catch (const std::exception& e) {
            return CHECK_ERROR(Runtime::setError(e.what()));
        } catch (...) {
            return CHECK_ERROR(Runtime::setError("unknown exception"));
        }
    }

    return CALL_RETURN_NULL;
}

result_t DBIterator::remove()
{
    if (g_now_context) {
        try {
            g_now_context->db_remove_i64(m_itr);

            obj_ptr<Table> table = (Table*)Table_base::getInstance(GetPrivate("table"));
            table->remove_index(m_id);
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

result_t DBIterator::update(exlib::string payer)
{
    if (g_now_context) {
        result_t hr;
        uint64_t pyr;

        hr = string_to_name(payer, pyr);
        if (hr < 0)
            return hr;

        v8::Local<v8::Value> var = GetPrivate("data");
        fc::variant v = fc_value(js_module_interface::g_isolate, var);
        bytes data;

        try {
            data = js_module_interface::g_module->m_abis.variant_to_binary(m_def.type, v,
                max_serialization_time);

            g_now_context->db_update_i64(m_itr, account_name(pyr), data.data(), data.size());

            obj_ptr<Table> table = (Table*)Table_base::getInstance(GetPrivate("table"));
            table->update_index(pyr, m_id, var);
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

result_t DBIterator::get_data(v8::Local<v8::Value>& retVal)
{
    retVal = GetPrivate("data");
    return 0;
}
}
