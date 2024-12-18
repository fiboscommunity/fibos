/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#pragma once

#include "../object.h"

namespace fibjs {

class bc_console_base : public object_base {
    DECLARE_CLASS(bc_console_base);

public:
    enum {
        C_FATAL = 0,
        C_ALERT = 1,
        C_CRIT = 2,
        C_ERROR = 3,
        C_WARN = 4,
        C_NOTICE = 5,
        C_INFO = 6,
        C_DEBUG = 7,
        C_PRINT = 9,
        C_NOTSET = 10
    };

public:
    // bc_console_base
    static result_t log(exlib::string fmt, OptArgs args);
    static result_t log(OptArgs args);
    static result_t debug(exlib::string fmt, OptArgs args);
    static result_t debug(OptArgs args);
    static result_t info(exlib::string fmt, OptArgs args);
    static result_t info(OptArgs args);
    static result_t notice(exlib::string fmt, OptArgs args);
    static result_t notice(OptArgs args);
    static result_t warn(exlib::string fmt, OptArgs args);
    static result_t warn(OptArgs args);
    static result_t error(exlib::string fmt, OptArgs args);
    static result_t error(OptArgs args);
    static result_t crit(exlib::string fmt, OptArgs args);
    static result_t crit(OptArgs args);
    static result_t alert(exlib::string fmt, OptArgs args);
    static result_t alert(OptArgs args);
    static result_t trace(exlib::string fmt, OptArgs args);
    static result_t trace(OptArgs args);
    static result_t dir(v8::Local<v8::Value> obj, v8::Local<v8::Object> options);
    static result_t _assert(v8::Local<v8::Value> value, exlib::string msg);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

    static result_t load(Isolate* isolate, v8::Local<v8::Value> v, obj_ptr<bc_console_base>& retVal)
    { return CALL_E_TYPEMISMATCH; }

public:
    static void s_static_log(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_debug(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_info(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_notice(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_warn(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_error(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_crit(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_alert(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_trace(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_dir(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static__assert(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

namespace fibjs {
inline ClassInfo& bc_console_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "log", s_static_log, true, ClassData::ASYNC_SYNC },
        { "debug", s_static_debug, true, ClassData::ASYNC_SYNC },
        { "info", s_static_info, true, ClassData::ASYNC_SYNC },
        { "notice", s_static_notice, true, ClassData::ASYNC_SYNC },
        { "warn", s_static_warn, true, ClassData::ASYNC_SYNC },
        { "error", s_static_error, true, ClassData::ASYNC_SYNC },
        { "crit", s_static_crit, true, ClassData::ASYNC_SYNC },
        { "alert", s_static_alert, true, ClassData::ASYNC_SYNC },
        { "trace", s_static_trace, true, ClassData::ASYNC_SYNC },
        { "dir", s_static_dir, true, ClassData::ASYNC_SYNC },
        { "assert", s_static__assert, true, ClassData::ASYNC_SYNC }
    };

    static ClassData::ClassConst s_const[] = {
        { "FATAL", C_FATAL },
        { "ALERT", C_ALERT },
        { "CRIT", C_CRIT },
        { "ERROR", C_ERROR },
        { "WARN", C_WARN },
        { "NOTICE", C_NOTICE },
        { "INFO", C_INFO },
        { "DEBUG", C_DEBUG },
        { "PRINT", C_PRINT },
        { "NOTSET", C_NOTSET }
    };

    static ClassData s_cd = {
        "bc_console", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, 0, NULL, ARRAYSIZE(s_const), s_const, NULL, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void bc_console_base::s_static_log(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(-1, 1);

    ARG(exlib::string, 0);
    ARG_LIST(1);

    hr = log(v0, v1);

    METHOD_OVER(-1, 0);

    ARG_LIST(0);

    hr = log(v0);

    METHOD_VOID();
}

inline void bc_console_base::s_static_debug(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(-1, 1);

    ARG(exlib::string, 0);
    ARG_LIST(1);

    hr = debug(v0, v1);

    METHOD_OVER(-1, 0);

    ARG_LIST(0);

    hr = debug(v0);

    METHOD_VOID();
}

inline void bc_console_base::s_static_info(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(-1, 1);

    ARG(exlib::string, 0);
    ARG_LIST(1);

    hr = info(v0, v1);

    METHOD_OVER(-1, 0);

    ARG_LIST(0);

    hr = info(v0);

    METHOD_VOID();
}

inline void bc_console_base::s_static_notice(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(-1, 1);

    ARG(exlib::string, 0);
    ARG_LIST(1);

    hr = notice(v0, v1);

    METHOD_OVER(-1, 0);

    ARG_LIST(0);

    hr = notice(v0);

    METHOD_VOID();
}

inline void bc_console_base::s_static_warn(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(-1, 1);

    ARG(exlib::string, 0);
    ARG_LIST(1);

    hr = warn(v0, v1);

    METHOD_OVER(-1, 0);

    ARG_LIST(0);

    hr = warn(v0);

    METHOD_VOID();
}

inline void bc_console_base::s_static_error(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(-1, 1);

    ARG(exlib::string, 0);
    ARG_LIST(1);

    hr = error(v0, v1);

    METHOD_OVER(-1, 0);

    ARG_LIST(0);

    hr = error(v0);

    METHOD_VOID();
}

inline void bc_console_base::s_static_crit(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(-1, 1);

    ARG(exlib::string, 0);
    ARG_LIST(1);

    hr = crit(v0, v1);

    METHOD_OVER(-1, 0);

    ARG_LIST(0);

    hr = crit(v0);

    METHOD_VOID();
}

inline void bc_console_base::s_static_alert(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(-1, 1);

    ARG(exlib::string, 0);
    ARG_LIST(1);

    hr = alert(v0, v1);

    METHOD_OVER(-1, 0);

    ARG_LIST(0);

    hr = alert(v0);

    METHOD_VOID();
}

inline void bc_console_base::s_static_trace(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(-1, 1);

    ARG(exlib::string, 0);
    ARG_LIST(1);

    hr = trace(v0, v1);

    METHOD_OVER(-1, 0);

    ARG_LIST(0);

    hr = trace(v0);

    METHOD_VOID();
}

inline void bc_console_base::s_static_dir(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(v8::Local<v8::Value>, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    hr = dir(v0, v1);

    METHOD_VOID();
}

inline void bc_console_base::s_static__assert(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(v8::Local<v8::Value>, 0);
    OPT_ARG(exlib::string, 1, "");

    hr = _assert(v0, v1);

    METHOD_VOID();
}
}
