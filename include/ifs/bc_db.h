/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#pragma once

#include "../object.h"

namespace fibjs {

class bc_db_base : public object_base {
    DECLARE_CLASS(bc_db_base);

public:
    // bc_db_base
    static result_t table(exlib::string scope, exlib::string code, exlib::string indexes);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

    static result_t load(Isolate* isolate, v8::Local<v8::Value> v, obj_ptr<bc_db_base>& retVal)
    { return CALL_E_TYPEMISMATCH; }

public:
    static void s_static_table(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

namespace fibjs {
inline ClassInfo& bc_db_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "table", s_static_table, true, ClassData::ASYNC_SYNC }
    };

    static ClassData s_cd = {
        "bc_db", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, 0, NULL, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void bc_db_base::s_static_table(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(3, 3);

    ARG(exlib::string, 0);
    ARG(exlib::string, 1);
    ARG(exlib::string, 2);

    hr = table(v0, v1, v2);

    METHOD_VOID();
}
}
