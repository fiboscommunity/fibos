/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#pragma once

#include "../object.h"

namespace fibjs {

class DBIterator_base;

class Table_base : public object_base {
    DECLARE_CLASS(Table_base);

public:
    // Table_base
    virtual result_t get_name(exlib::string& retVal) = 0;
    virtual result_t get_code(exlib::string& retVal) = 0;
    virtual result_t get_scope(exlib::string& retVal) = 0;
    virtual result_t emplace(exlib::string payer, v8::Local<v8::Object> val) = 0;
    virtual result_t find(v8::Local<v8::Value> id, obj_ptr<DBIterator_base>& retVal) = 0;
    virtual result_t get_primary_key(v8::Local<v8::Value>& retVal) = 0;
    virtual result_t begin(obj_ptr<DBIterator_base>& retVal) = 0;
    virtual result_t end(obj_ptr<DBIterator_base>& retVal) = 0;
    virtual result_t lowerbound(v8::Local<v8::Value> id, obj_ptr<DBIterator_base>& retVal) = 0;
    virtual result_t upperbound(v8::Local<v8::Value> id, obj_ptr<DBIterator_base>& retVal) = 0;
    virtual result_t get_indexes(v8::Local<v8::Object>& retVal) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

    static result_t load(Isolate* isolate, v8::Local<v8::Value> v, obj_ptr<Table_base>& retVal)
    { return CALL_E_TYPEMISMATCH; }


public:
    static void s_get_name(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_code(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_scope(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_emplace(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_find(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_primary_key(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_begin(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_end(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_lowerbound(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_upperbound(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_indexes(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

#include "ifs/DBIterator.h"

namespace fibjs {
inline ClassInfo& Table_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "emplace", s_emplace, false, ClassData::ASYNC_SYNC },
        { "find", s_find, false, ClassData::ASYNC_SYNC },
        { "get_primary_key", s_get_primary_key, false, ClassData::ASYNC_SYNC },
        { "begin", s_begin, false, ClassData::ASYNC_SYNC },
        { "end", s_end, false, ClassData::ASYNC_SYNC },
        { "lowerbound", s_lowerbound, false, ClassData::ASYNC_SYNC },
        { "upperbound", s_upperbound, false, ClassData::ASYNC_SYNC }
    };

    static ClassData::ClassProperty s_property[] = {
        { "name", s_get_name, block_set, false },
        { "code", s_get_code, block_set, false },
        { "scope", s_get_scope, block_set, false },
        { "indexes", s_get_indexes, block_set, false }
    };

    static ClassData s_cd = {
        "Table", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void Table_base::s_get_name(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(Table_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_name(vr);

    METHOD_RETURN();
}

inline void Table_base::s_get_code(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(Table_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_code(vr);

    METHOD_RETURN();
}

inline void Table_base::s_get_scope(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(Table_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_scope(vr);

    METHOD_RETURN();
}

inline void Table_base::s_emplace(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Table_base);
    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(v8::Local<v8::Object>, 1);

    hr = pInst->emplace(v0, v1);

    METHOD_VOID();
}

inline void Table_base::s_find(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<DBIterator_base> vr;

    METHOD_INSTANCE(Table_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = pInst->find(v0, vr);

    METHOD_RETURN();
}

inline void Table_base::s_get_primary_key(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Value> vr;

    METHOD_INSTANCE(Table_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_primary_key(vr);

    METHOD_RETURN();
}

inline void Table_base::s_begin(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<DBIterator_base> vr;

    METHOD_INSTANCE(Table_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->begin(vr);

    METHOD_RETURN();
}

inline void Table_base::s_end(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<DBIterator_base> vr;

    METHOD_INSTANCE(Table_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->end(vr);

    METHOD_RETURN();
}

inline void Table_base::s_lowerbound(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<DBIterator_base> vr;

    METHOD_INSTANCE(Table_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = pInst->lowerbound(v0, vr);

    METHOD_RETURN();
}

inline void Table_base::s_upperbound(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<DBIterator_base> vr;

    METHOD_INSTANCE(Table_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = pInst->upperbound(v0, vr);

    METHOD_RETURN();
}

inline void Table_base::s_get_indexes(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

    METHOD_INSTANCE(Table_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_indexes(vr);

    METHOD_RETURN();
}
}
