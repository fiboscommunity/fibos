/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#pragma once

#include "../object.h"

namespace fibjs {

class Buffer_base;

class bc_crypto_base : public object_base {
    DECLARE_CLASS(bc_crypto_base);

public:
    // bc_crypto_base
    static result_t recover_key(exlib::string digest, exlib::string signature, exlib::string& retVal);
    static result_t sha1(Buffer_base* data, exlib::string& retVal);
    static result_t sha256(Buffer_base* data, exlib::string& retVal);
    static result_t sha512(Buffer_base* data, exlib::string& retVal);
    static result_t ripemd160(Buffer_base* data, exlib::string& retVal);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

    static result_t load(Isolate* isolate, v8::Local<v8::Value> v, obj_ptr<bc_crypto_base>& retVal)
    { return CALL_E_TYPEMISMATCH; }

public:
    static void s_static_recover_key(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_sha1(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_sha256(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_sha512(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_ripemd160(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

#include "ifs/Buffer.h"

namespace fibjs {
inline ClassInfo& bc_crypto_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "recover_key", s_static_recover_key, true, ClassData::ASYNC_SYNC },
        { "sha1", s_static_sha1, true, ClassData::ASYNC_SYNC },
        { "sha256", s_static_sha256, true, ClassData::ASYNC_SYNC },
        { "sha512", s_static_sha512, true, ClassData::ASYNC_SYNC },
        { "ripemd160", s_static_ripemd160, true, ClassData::ASYNC_SYNC }
    };

    static ClassData s_cd = {
        "bc_crypto", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, 0, NULL, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void bc_crypto_base::s_static_recover_key(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(exlib::string, 1);

    hr = recover_key(v0, v1, vr);

    METHOD_RETURN();
}

inline void bc_crypto_base::s_static_sha1(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(obj_ptr<Buffer_base>, 0);

    hr = sha1(v0, vr);

    METHOD_RETURN();
}

inline void bc_crypto_base::s_static_sha256(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(obj_ptr<Buffer_base>, 0);

    hr = sha256(v0, vr);

    METHOD_RETURN();
}

inline void bc_crypto_base::s_static_sha512(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(obj_ptr<Buffer_base>, 0);

    hr = sha512(v0, vr);

    METHOD_RETURN();
}

inline void bc_crypto_base::s_static_ripemd160(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(obj_ptr<Buffer_base>, 0);

    hr = ripemd160(v0, vr);

    METHOD_RETURN();
}
}
