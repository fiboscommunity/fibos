/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#pragma once

#include "../object.h"

namespace fibjs {

class trans_base : public object_base {
    DECLARE_CLASS(trans_base);

public:
    // trans_base
    static result_t send_inline(exlib::string account, exlib::string name, v8::Local<v8::Object> args, v8::Local<v8::Array> authorization);
    static result_t send_context_free_inline(exlib::string account, exlib::string name, v8::Local<v8::Object> args);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

    static result_t load(Isolate* isolate, v8::Local<v8::Value> v, obj_ptr<trans_base>& retVal)
    { return CALL_E_TYPEMISMATCH; }

public:
    static void s_static_send_inline(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_send_context_free_inline(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

namespace fibjs {
inline ClassInfo& trans_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "send_inline", s_static_send_inline, true, ClassData::ASYNC_SYNC },
        { "send_context_free_inline", s_static_send_context_free_inline, true, ClassData::ASYNC_SYNC }
    };

    static ClassData s_cd = {
        "trans", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, 0, NULL, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void trans_base::s_static_send_inline(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(4, 3);

    ARG(exlib::string, 0);
    ARG(exlib::string, 1);
    ARG(v8::Local<v8::Object>, 2);
    OPT_ARG(v8::Local<v8::Array>, 3, v8::Array::New(isolate->m_isolate));

    hr = send_inline(v0, v1, v2, v3);

    METHOD_VOID();
}

inline void trans_base::s_static_send_context_free_inline(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(3, 3);

    ARG(exlib::string, 0);
    ARG(exlib::string, 1);
    ARG(v8::Local<v8::Object>, 2);

    hr = send_context_free_inline(v0, v1, v2);

    METHOD_VOID();
}
}
