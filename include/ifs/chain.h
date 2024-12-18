/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#pragma once

#include "../object.h"
#include "ifs/EventEmitter.h"

namespace fibjs {

class EventEmitter_base;

class chain_base : public EventEmitter_base {
    DECLARE_CLASS(chain_base);

public:
    // chain_base
    static result_t get_name(exlib::string& retVal);
    static result_t set_name(exlib::string newVal);
    static result_t get_data_dir(exlib::string& retVal);
    static result_t set_data_dir(exlib::string newVal);
    static result_t get_config_dir(exlib::string& retVal);
    static result_t set_config_dir(exlib::string newVal);
    static result_t get_log_level(exlib::string& retVal);
    static result_t set_log_level(exlib::string newVal);
    static result_t get_pubkey_prefix(exlib::string& retVal);
    static result_t set_pubkey_prefix(exlib::string newVal);
    static result_t load(exlib::string name, v8::Local<v8::Object> cfg);
    static result_t load(v8::Local<v8::Object> cfgs);
    static result_t start();
    static result_t stop();
    static result_t is_dirty();
    static result_t get_enableJSContract(bool& retVal);
    static result_t set_enableJSContract(bool newVal);
    static result_t post(exlib::string resource, exlib::string body, exlib::string& retVal, AsyncEvent* ac);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

    static result_t load(Isolate* isolate, v8::Local<v8::Value> v, obj_ptr<chain_base>& retVal)
    { return CALL_E_TYPEMISMATCH; }

public:
    static void s_static_get_name(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_set_name(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_data_dir(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_set_data_dir(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_config_dir(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_set_config_dir(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_log_level(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_set_log_level(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_pubkey_prefix(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_set_pubkey_prefix(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_load(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_start(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_stop(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_is_dirty(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_enableJSContract(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_set_enableJSContract(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_post(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_STATICVALUE3(chain_base, post, exlib::string, exlib::string, exlib::string);
};
}

namespace fibjs {
inline ClassInfo& chain_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "load", s_static_load, true, ClassData::ASYNC_SYNC },
        { "start", s_static_start, true, ClassData::ASYNC_SYNC },
        { "stop", s_static_stop, true, ClassData::ASYNC_SYNC },
        { "is_dirty", s_static_is_dirty, true, ClassData::ASYNC_SYNC },
        { "post", s_static_post, true, ClassData::ASYNC_SYNC },
    };

    static ClassData::ClassProperty s_property[] = {
        { "name", s_static_get_name, s_static_set_name, true },
        { "data_dir", s_static_get_data_dir, s_static_set_data_dir, true },
        { "config_dir", s_static_get_config_dir, s_static_set_config_dir, true },
        { "log_level", s_static_get_log_level, s_static_set_log_level, true },
        { "pubkey_prefix", s_static_get_pubkey_prefix, s_static_set_pubkey_prefix, true },
        { "enableJSContract", s_static_get_enableJSContract, s_static_set_enableJSContract, true },
    };

    static ClassData s_cd = {
        "chain", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &EventEmitter_base::class_info(),
        true
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void chain_base::s_static_get_name(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_name(vr);

    METHOD_RETURN();
}

inline void chain_base::s_static_set_name(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = set_name(v0);

    METHOD_VOID();
}

inline void chain_base::s_static_get_data_dir(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_data_dir(vr);

    METHOD_RETURN();
}

inline void chain_base::s_static_set_data_dir(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = set_data_dir(v0);

    METHOD_VOID();
}

inline void chain_base::s_static_get_config_dir(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_config_dir(vr);

    METHOD_RETURN();
}

inline void chain_base::s_static_set_config_dir(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = set_config_dir(v0);

    METHOD_VOID();
}

inline void chain_base::s_static_get_log_level(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_log_level(vr);

    METHOD_RETURN();
}

inline void chain_base::s_static_set_log_level(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = set_log_level(v0);

    METHOD_VOID();
}

inline void chain_base::s_static_get_pubkey_prefix(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_pubkey_prefix(vr);

    METHOD_RETURN();
}

inline void chain_base::s_static_set_pubkey_prefix(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = set_pubkey_prefix(v0);

    METHOD_VOID();
}

inline void chain_base::s_static_load(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    hr = load(v0, v1);

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Object>, 0);

    hr = load(v0);

    METHOD_VOID();
}

inline void chain_base::s_static_start(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = start();

    METHOD_VOID();
}

inline void chain_base::s_static_stop(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = stop();

    METHOD_VOID();
}

inline void chain_base::s_static_is_dirty(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = is_dirty();

    METHOD_VOID();
}

inline void chain_base::s_static_get_enableJSContract(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_enableJSContract(vr);

    METHOD_RETURN();
}

inline void chain_base::s_static_set_enableJSContract(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(1, 1);
    ARG(bool, 0);

    hr = set_enableJSContract(v0);

    METHOD_VOID();
}

inline void chain_base::s_static_post(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    ASYNC_METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(exlib::string, 1, "");

    if (!cb.IsEmpty())
        hr = acb_post(v0, v1, cb, args);
    else
        hr = ac_post(v0, v1, vr);

    METHOD_RETURN();
}
}
