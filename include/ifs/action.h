/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#pragma once

#include "../object.h"

namespace fibjs {

class action_base : public object_base {
    DECLARE_CLASS(action_base);

public:
    // action_base
    static result_t get_name(exlib::string& retVal);
    static result_t get_account(exlib::string& retVal);
    static result_t get_receiver(exlib::string& retVal);
    static result_t get_publication_time(double& retVal);
    static result_t get_authorization(v8::Local<v8::Array>& retVal);
    static result_t get_id(exlib::string& retVal);
    static result_t is_account(exlib::string name, bool& retVal);
    static result_t has_recipient(exlib::string name, bool& retVal);
    static result_t require_recipient(exlib::string name);
    static result_t has_auth(exlib::string name, bool& retVal);
    static result_t require_auth(exlib::string name, exlib::string permission);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

    static result_t load(Isolate* isolate, v8::Local<v8::Value> v, obj_ptr<action_base>& retVal)
    { return CALL_E_TYPEMISMATCH; }

public:
    static void s_static_get_name(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_account(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_receiver(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_publication_time(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_authorization(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_id(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_is_account(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_has_recipient(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_require_recipient(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_has_auth(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_require_auth(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

namespace fibjs {
inline ClassInfo& action_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "is_account", s_static_is_account, true, ClassData::ASYNC_SYNC },
        { "has_recipient", s_static_has_recipient, true, ClassData::ASYNC_SYNC },
        { "require_recipient", s_static_require_recipient, true, ClassData::ASYNC_SYNC },
        { "has_auth", s_static_has_auth, true, ClassData::ASYNC_SYNC },
        { "require_auth", s_static_require_auth, true, ClassData::ASYNC_SYNC }
    };

    static ClassData::ClassProperty s_property[] = {
        { "name", s_static_get_name, block_set, true },
        { "account", s_static_get_account, block_set, true },
        { "receiver", s_static_get_receiver, block_set, true },
        { "publication_time", s_static_get_publication_time, block_set, true },
        { "authorization", s_static_get_authorization, block_set, true },
        { "id", s_static_get_id, block_set, true }
    };

    static ClassData s_cd = {
        "action", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void action_base::s_static_get_name(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_name(vr);

    METHOD_RETURN();
}

inline void action_base::s_static_get_account(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_account(vr);

    METHOD_RETURN();
}

inline void action_base::s_static_get_receiver(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_receiver(vr);

    METHOD_RETURN();
}

inline void action_base::s_static_get_publication_time(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    double vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_publication_time(vr);

    METHOD_RETURN();
}

inline void action_base::s_static_get_authorization(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Array> vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_authorization(vr);

    METHOD_RETURN();
}

inline void action_base::s_static_get_id(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_id(vr);

    METHOD_RETURN();
}

inline void action_base::s_static_is_account(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = is_account(v0, vr);

    METHOD_RETURN();
}

inline void action_base::s_static_has_recipient(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = has_recipient(v0, vr);

    METHOD_RETURN();
}

inline void action_base::s_static_require_recipient(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = require_recipient(v0);

    METHOD_VOID();
}

inline void action_base::s_static_has_auth(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = has_auth(v0, vr);

    METHOD_RETURN();
}

inline void action_base::s_static_require_auth(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(exlib::string, 1, "");

    hr = require_auth(v0, v1);

    METHOD_VOID();
}
}
