#include "config.hpp"

#include "object.h"
#include "Buffer.h"
#include "ifs/bc_crypto.h"
#include "../include/js_module.h"
#include <eosio/chain/transaction_context.hpp>
#include <fc/crypto/sha256.hpp>
#include <fc/crypto/sha1.hpp>
#include <fc/crypto/signature.hpp>

using namespace eosio::chain;

namespace fibjs {

result_t bc_crypto_base::recover_key(exlib::string digest, exlib::string signature, exlib::string& retVal)
{
    try {
        fc::crypto::signature sig = fc::crypto::signature(std::string(signature));
        fc::crypto::public_key pk(sig, fc::sha256(digest.c_str()), false);
        retVal = pk.to_string([]() {});
    } catch (const fc::exception& e) {
        return CHECK_ERROR(Runtime::setError(e.to_detail_string()));
    } catch (...) {
        return CHECK_ERROR(Runtime::setError("unknown exception"));
    }

    return 0;
}

template <class Encoder>
exlib::string encode(Buffer_base* buf)
{
    Buffer* buff = (Buffer*)buf;

    char* data = (char*)buff->data();
    size_t datalen = buff->length();

    Encoder e;
    const size_t bs = config::hashing_checktime_block_size;
    while (datalen > bs) {
        e.write(data, bs);
        data += bs;
        datalen -= bs;
        g_now_context->trx_context.checktime();
    }
    e.write(data, datalen);
    auto r = e.result();

    obj_ptr<Buffer_base> buf1 = new Buffer(r.data(), r.data_size());
    exlib::string rs;
    buf1->hex(rs);

    return rs;
}

result_t bc_crypto_base::sha1(Buffer_base* data, exlib::string& retVal)
{
    try {
        retVal = encode<fc::sha1::encoder>(data);
    } catch (const chain_exception& e) {
        return CHECK_ERROR(Runtime::setError(e.to_detail_string()));
    } catch (...) {
        return CHECK_ERROR(Runtime::setError("unknown exception"));
    }

    return 0;
}

result_t bc_crypto_base::sha256(Buffer_base* data, exlib::string& retVal)
{
    try {
        retVal = encode<fc::sha256::encoder>(data);
    } catch (const chain_exception& e) {
        return CHECK_ERROR(Runtime::setError(e.to_detail_string()));
    } catch (...) {
        return CHECK_ERROR(Runtime::setError("unknown exception"));
    }

    return 0;
}

result_t bc_crypto_base::sha512(Buffer_base* data, exlib::string& retVal)
{
    try {
        retVal = encode<fc::sha512::encoder>(data);
    } catch (const chain_exception& e) {
        return CHECK_ERROR(Runtime::setError(e.to_detail_string()));
    } catch (...) {
        return CHECK_ERROR(Runtime::setError("unknown exception"));
    }

    return 0;
}

result_t bc_crypto_base::ripemd160(Buffer_base* data, exlib::string& retVal)
{
    try {
        retVal = encode<fc::ripemd160::encoder>(data);
    } catch (const chain_exception& e) {
        return CHECK_ERROR(Runtime::setError(e.to_detail_string()));
    } catch (...) {
        return CHECK_ERROR(Runtime::setError("unknown exception"));
    }

    return 0;
}
} // namespace fibjs