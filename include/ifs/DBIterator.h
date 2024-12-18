/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#pragma once


#include "../object.h"

namespace fibjs {

class DBIterator_base : public object_base {
    DECLARE_CLASS(DBIterator_base);

public:
    // DBIterator_base
    virtual result_t is_begin(bool& retVal) = 0;
    virtual result_t is_end(bool& retVal) = 0;
    virtual result_t next(obj_ptr<DBIterator_base>& retVal) = 0;
    virtual result_t previous(obj_ptr<DBIterator_base>& retVal) = 0;
    virtual result_t remove() = 0;
    virtual result_t update(exlib::string payer) = 0;
    virtual result_t get_data(v8::Local<v8::Value>& retVal) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

    static result_t load(Isolate* isolate, v8::Local<v8::Value> v, obj_ptr<DBIterator_base>& retVal)
    { return CALL_E_TYPEMISMATCH; }


public:
    static void s_is_begin(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_is_end(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_next(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_previous(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_remove(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_update(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_data(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

namespace fibjs {
inline ClassInfo& DBIterator_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "is_begin", s_is_begin, false, ClassData::ASYNC_SYNC },
        { "is_end", s_is_end, false, ClassData::ASYNC_SYNC },
        { "next", s_next, false, ClassData::ASYNC_SYNC },
        { "previous", s_previous, false, ClassData::ASYNC_SYNC },
        { "remove", s_remove, false, ClassData::ASYNC_SYNC },
        { "update", s_update, false, ClassData::ASYNC_SYNC }
    };

    static ClassData::ClassProperty s_property[] = {
        { "data", s_get_data, block_set, false }
    };

    static ClassData s_cd = {
        "DBIterator", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void DBIterator_base::s_is_begin(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(DBIterator_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->is_begin(vr);

    METHOD_RETURN();
}

inline void DBIterator_base::s_is_end(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(DBIterator_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->is_end(vr);

    METHOD_RETURN();
}

inline void DBIterator_base::s_next(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<DBIterator_base> vr;

    METHOD_INSTANCE(DBIterator_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->next(vr);

    METHOD_RETURN();
}

inline void DBIterator_base::s_previous(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<DBIterator_base> vr;

    METHOD_INSTANCE(DBIterator_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->previous(vr);

    METHOD_RETURN();
}

inline void DBIterator_base::s_remove(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(DBIterator_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->remove();

    METHOD_VOID();
}

inline void DBIterator_base::s_update(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(DBIterator_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->update(v0);

    METHOD_VOID();
}

inline void DBIterator_base::s_get_data(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Value> vr;

    METHOD_INSTANCE(DBIterator_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);


    hr = pInst->get_data(vr);

    METHOD_RETURN();
}
}
