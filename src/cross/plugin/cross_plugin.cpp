/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */

#include "config.hpp"
#include "../include/Header.hpp"
#include "../include/keccak.h"
#include "../include/datasize.h"
#include "../include/RLP.hpp"
#include "../include/hex.hpp"
#include "../include/BaseConverter.hpp"
#include "../include/InfInt.h"
#include "../include/cross_struct.hpp"
#include "../include/global_var.hpp"
#include "../include/cross_plugin.h"
#include <fc/io/json.hpp>
#include <boost/smart_ptr.hpp>
#include <eosio/chain/apply_context.hpp>
#include <blst/include/blst.h>
#include "types.h"
#include <fc/crypto/public_key.hpp>
#include <eosio/chain/asset.hpp>
#include <eosio/chain/permission_link_object.hpp>
#include <eosio/producer_plugin/producer_plugin.hpp>

using namespace eosio::chain;
using namespace fibos;

namespace fc {
namespace crypto {
    fibos::public_key_point_data serialize_ecc_point(public_key key);
};
}; // namespace fc

namespace eosio {

using namespace eosio::chain;
using mvo = fc::mutable_variant_object;

static const uint32_t default_expiration_delta = 120;
static const chain::name function_name = "sign"_n;

struct signature_info {
    account_name signee;
    signature_type signature;
};

template <typename T>
std::string parse_number_to_string(T value)
{
    const size_t byte_length = sizeof(T) / sizeof(char);
    std::string result;
    for (size_t i = 0; i < byte_length; i++) {
        result.append(1, value & 0xFF);
        value = value >> 8;
    }
    std::string reserve_result;
    reserve_result.insert(reserve_result.end(), result.rbegin(), result.rend());
    return reserve_result;
}

struct ethash_block_number_object : public chainbase::object<ethash_block_number_object_type, ethash_block_number_object> {
    OBJECT_CTOR(ethash_block_number_object);

    id_type id;
    uint64_t epoch;
};
using ethash_block_number_id_type = ethash_block_number_object::id_type;

struct by_ethash_epoch;

using ethash_block_number_index = chainbase::shared_multi_index_container<
    ethash_block_number_object,
    indexed_by<
        ordered_unique<tag<by_id>, member<ethash_block_number_object, ethash_block_number_object::id_type, &ethash_block_number_object::id>>,
        ordered_unique<tag<by_ethash_epoch>, member<ethash_block_number_object, uint64_t, &ethash_block_number_object::epoch>>>>;
}; // namespace eosio

CHAINBASE_SET_INDEX_TYPE(eosio::ethash_block_number_object, eosio::ethash_block_number_index)

namespace eosio {

const std::string eth_key = "ethash";
const std::string cross_key = "cross";
constexpr uint64_t epoch_vec_size = 5;
constexpr uint64_t update_epoch_pos = 2;

class cross_plugin_impl {
public:
    cross_plugin_impl(boost::asio::io_service& io) : _timer(io) { }

    ~cross_plugin_impl()
    {
    }

    uint64_t lower_epoch = 0;
    std::vector<boost::shared_array<uint32_t>> cache_vec;
    boost::program_options::variables_map _options;
    std::mutex epoch_mutex;
    chain_plugin* chain_plug = nullptr;
    bool is_generating = false;
    account_name _producer;
    public_key_type _sign_public_key;
    private_key_type _sign_private_key;
    boost::asio::deadline_timer _timer;
    bool init_check = false;
    time_point _start_time = fc::time_point::now();
    std::map<public_key_type, private_key_type> _signature_providers;
    uint64_t sign_permission = config::active_name.to_uint64_t();

    std::optional<action> fill_action(account_name code, action_name acttype, vector<permission_level> auths, const fc::variant& data)
    {
        try {
            auto abi_serializer_max_time = app().get_plugin<chain_plugin>().get_abi_serializer_max_time();
            const auto& acnt = chain_plug->chain().get_account(code);
            auto abi = acnt.get_abi();
            chain::abi_serializer abis(abi, abi_serializer_max_time);

            string action_type_name = abis.get_action_type(acttype);
            FC_ASSERT(action_type_name != string(), "unknown action type ${a}", ("a", acttype));

            action act;
            act.account = code;
            act.name = acttype;
            act.authorization = auths;
            act.data = abis.variant_to_binary(action_type_name, data, abi_serializer_max_time);
            return act;
        }
        FC_LOG_AND_DROP()
        return std::optional<action>();
    }


    void set_transaction_headers(transaction& trx, uint32_t expiration = default_expiration_delta, uint32_t delay_sec = 0)
    {
        trx.expiration = time_point_sec { chain_plug->chain().head_block_time() + fc::seconds(expiration) };
        trx.set_reference_block(chain_plug->chain().last_irreversible_block_id());

        trx.max_net_usage_words = 0; // No limit
        trx.max_cpu_usage_ms = 0; // No limit
        trx.delay_sec = delay_sec;
    }

    std::optional<signed_transaction> generate_signed_transaction_from_action(action actn)
    {
        if (!_producer) {
            elog("can't find producer!");
            return std::optional<signed_transaction>();
        }
        signed_transaction trx;
        trx.actions.emplace_back(actn);
        set_transaction_headers(trx);
        trx.sign(_sign_private_key, chain_plug->chain().get_chain_id());
        return trx;
    }

    void push_action(action actn)
    {
        auto trx_opt = generate_signed_transaction_from_action(actn);
        if (trx_opt) {
            auto wrapped_next = [](const next_function_variant<transaction_trace_ptr>& result) {
                if (std::holds_alternative<fc::exception_ptr>(result)) {
                    auto& e = std::get<fc::exception_ptr>(result);
                    if (e->code() == 3040008)
                        wlog("${e}", ("e", e->to_detail_string())); // tx_duplicate
                    else
                        elog("${e}", ("e", e->to_detail_string()));
                }
            };
            auto& http = app().get_plugin<http_plugin>();
            fc::microseconds max_response_time = http.get_max_response_time();

            shared_ptr<packed_transaction> packed_trx = std::make_shared<packed_transaction>(*trx_opt);
            app().get_method<plugin_interface::incoming::methods::transaction_async>()(
                packed_trx, true, transaction_metadata::trx_type::input, false, wrapped_next);
        } else {
            elog("push_action failed, for generate_signed_transaction_from_action failed");
        }
    }

    private_key_type get_valid_private_key()
    {
        auto& chain = chain_plug->chain();
        const auto& active_producers = chain.active_producers();
        for (const auto& producer : active_producers.producers) {
            if (producer.producer_name != _producer)
                continue;
            auto iter = _signature_providers.find(std::get<0>(producer.authority).keys.front().key);
            if (iter != _signature_providers.end())
                return iter->second;
        }
        return private_key_type();
    }

    bool do_init_check()
    {
        if (chain_plug->chain().head_block_state()->header.timestamp.to_time_point() < _start_time)
            return false;

        auto create_permission = config::active_name;
        auto& chain = chain_plug->chain();
        chainbase::database& db = const_cast<chainbase::database&>(chain.db());
        auto link_key = boost::make_tuple(_producer, chain::config::eosio_cross_name, function_name);
        auto link = db.find<permission_link_object, by_action_name>(link_key);
        if (link)
            create_permission = link->required_permission;

        auto& http = app().get_plugin<http_plugin>();
        fc::microseconds max_response_time = http.get_max_response_time();

        chain_apis::read_only::get_account_params p;
        p.account_name = _producer;
        eosio::chain_apis::read_only::get_account_results result = std::get<eosio::chain_apis::read_only::get_account_results>(chain_plug->get_read_only_api(max_response_time).get_account(p, fc::time_point::maximum())());
        for (chain_apis::permission perm : result.permissions) {
            if ((perm.perm_name != config::active_name) && (perm.perm_name != create_permission))
                continue;
            for (key_weight key_wei : perm.required_auth.keys) {
                if (key_wei.key != _sign_public_key)
                    continue;
                if (key_wei.weight >= perm.required_auth.threshold) {
                    sign_permission = perm.perm_name.to_uint64_t();
                    return true;
                }
            }
        }
        return false;
    }

    void handle_sign_action()
    {
        chain_apis::read_only::get_table_rows_result result;
        private_key_type producer_private_key = get_valid_private_key();
        if (producer_private_key == private_key_type())
            return;

        auto& chain = chain_plug->chain();
        auto lib_num = chain.last_irreversible_block_num();
        auto irr_time = chain.fetch_block_by_number(lib_num)->timestamp.to_time_point();

        chain_apis::read_only::get_table_rows_params p;
        p.json = true;
        p.code = chain::config::eosio_cross_name;
        p.scope = chain::config::eosio_cross_name.to_string();
        p.table = "signpool"_n;
        p.limit = 500;

        p.lower_bound = std::to_string(0);

        auto need_sign = [this](std::vector<signature_info> sig_infos) {
            for (const signature_info& sig_info : sig_infos) {
                if (sig_info.signee == this->_producer && sig_info.signature == signature_type())
                    return true;
            }
            return false;
        };

        auto& http = app().get_plugin<http_plugin>();
        fc::microseconds max_response_time = http.get_max_response_time();

        uint64_t primary = 0;
        do {
            result = std::get<chain_apis::read_only::get_table_rows_result>(chain_plug->get_read_only_api(max_response_time).get_table_rows(p, fc::time_point::maximum())());
            for (auto ret : result.rows) {
                primary = ret["primary"].as<uint64_t>();
                sha256 digest = ret["digest"].as<sha256>();
                std::vector<signature_info> signatures = ret["signatures"].as<std::vector<signature_info>>();
                uint64_t timestamp = ret["timestamp"].as<uint64_t>();
                if (irr_time < time_point(microseconds(timestamp * 1000000)))
                    continue;

                if (!need_sign(signatures))
                    continue;

                auto result = producer_private_key.sign(digest);
                auto actn = fill_action(chain::config::eosio_cross_name, function_name, vector<permission_level> { { name(_producer), name(sign_permission) } }, mvo()("producer", _producer)("primary", primary)("signature", result));
                if (actn)
                    push_action(*actn);
            }
            p.lower_bound = std::to_string(primary);
        } while (result.more);
    }

    void handle_action_loop()
    {
        try {
            _timer.expires_from_now(boost::posix_time::seconds(1));
            app().executor().post(priority::lowest, exec_queue::read_only, [&]() {
                handle_action_loop();
            });

            if (!init_check) {
                if (do_init_check())
                    init_check = true;
                else
                    return;
            }

            handle_sign_action();
        } catch (...) {}
    }

    std::pair<bool, uint64_t> get_db_epoch()
    {
        auto& chain = chain_plug->chain();
        chainbase::database& db = const_cast<chainbase::database&>(chain.db());
        const auto& idx = db.get_index<ethash_block_number_index, by_id>();
        if (idx.begin() == idx.end()) {
            elog("Doesn't init epoch, can't get db");
            return std::make_pair(false, 0);
        }
        return std::make_pair(true, idx.begin()->epoch);
    }

    hash_256 genseed(uint64_t block_num)
    {
        uint64_t epoch = block_num / BLOCKS_PER_EPOCH;
        hash_256 seed;

        for (int i = 0; i < 32; i++)
            seed.hash[i] = 0;
        for (int i = 0; i < epoch; i++)
            keccak_256(&seed.hash[0], 32, &seed.hash[0], 32);

        return seed;
    }

    boost::shared_array<uint32_t> gencache(uint64_t block_num)
    {
        hash_256 seed = genseed(block_num);
        uint64_t epoch = block_num / BLOCKS_PER_EPOCH;
        uint64_t size = datasetSizes[epoch];
        uint64_t index = size / 64;
        uint64_t part = size % 64;

        std::vector<uint8_t> cache(size, 0);

        keccak_512(&cache[0], 64, &seed.hash[0], 32);

        for (int i = 1; i < size / 64; i++)
            keccak_512(&cache[i * 64], 64, &cache[(i - 1) * 64], 64);
        keccak_512(&cache[size - part], part, &cache[size - part - 64], 64);

        int64_t rows = size / 64;
        for (int i = 0; i < 3; i++) {
            uint32_t srcOff = 0;
            uint32_t dstOff = 0;
            uint32_t xorOff = 0;
            for (int j = 0; j < rows; j++) {
                srcOff = ((j - 1 + rows) % rows) * 64;
                dstOff = j * 64;
                uint32_t i32 = cache[dstOff] | (cache[dstOff + 1] << 8) | (cache[dstOff + 2] << 16) | (cache[dstOff + 3] << 24);
                xorOff = (i32) % rows * 64;

                uint8_t XOR[64];
                for (int k = 0; k < 64; k++)
                    XOR[k] = cache[srcOff + k] ^ cache[xorOff + k];
                keccak_512(&cache[dstOff], 64, &XOR[0], 64);
            }
        }

        boost::shared_array<uint32_t> result(new uint32_t[size / 4]);

        for (int i = 0; i < size / 4; i++)
            result[i] = cache[i * 4] | (cache[i * 4 + 1] << 8) | (cache[i * 4 + 2] << 16) | (cache[i * 4 + 3] << 24);

        return result;
    }

    uint32_t fnv(uint32_t a, uint32_t b)
    {
        return a * 0x01000193 ^ b;
    }

    void fnvHash(std::vector<uint32_t>& mix, uint32_t* data, int len)
    {
        for (int i = 0; i < len; i++)
            mix[i] = mix[i] * 0x01000193 ^ data[i];
    }

    boost::shared_array<uint32_t> lookup(uint64_t epoch, uint32_t index, uint32_t* cache)
    {
        int cache_length = datasetSizes[epoch] / 4;
        int hashWords = 16;
        int hashBytes = 64;
        int datasetParents = 256;
        uint32_t rows = cache_length / hashWords;
        uint32_t t = cache[(index % rows) * hashWords] ^ index;
        std::vector<uint8_t> mix(hashBytes, 0);
        uint8_t* p = (uint8_t*)&t;

        for (int i = 0; i < 4; i++)
            mix[i] = p[i];

        for (int i = 1; i < 16; i++) {
            uint32_t tt = cache[(index % rows) * hashWords + uint32_t(i)];
            uint8_t* pp = (uint8_t*)&tt;
            for (int j = 0; j < 4; j++)
                mix[i * 4 + j] = pp[j];
        }

        keccak_512(&mix[0], 64, &mix[0], 64);
        std::vector<uint32_t> intMix(hashWords, 0);

        for (int i = 0; i < hashWords; i++)
            intMix[i] = mix[i * 4] | (mix[i * 4 + 1] << 8) | (mix[i * 4 + 2] << 16) | (mix[i * 4 + 3] << 24);

        for (uint32_t i = 0; i < datasetParents; i++) {
            uint32_t parent = fnv(index ^ i, intMix[i % 16]) % rows;
            fnvHash(intMix, &cache[parent * hashWords], hashWords);
        }

        keccak_512(&mix[0], 64, (uint8_t*)&intMix[0], 64);
        boost::shared_array<uint32_t> data(new uint32_t[hashWords]);

        for (int i = 0; i < hashWords; i++)
            data[i] = mix[i * 4] | (mix[i * 4 + 1] << 8) | (mix[i * 4 + 2] << 16) | (mix[i * 4 + 3] << 24);

        return data;
    }

    std::pair<hash_256, hash_256> hashimoto(uint64_t epoch, uint32_t* cache, hash_256 hash, uint64_t nonce)
    {
        int mixBytes = 128;
        int hashBytes = 64;
        int loopAccesses = 64;

        uint64_t size = cacheSizes[epoch];

        std::vector<uint8_t> seed(40, 0);
        uint32_t rows = uint32_t(size / mixBytes);

        for (int i = 0; i < 32; i++)
            seed[i] = hash.hash[i];

        uint8_t* p = (uint8_t*)&nonce;
        for (int i = 0; i < 8; i++)
            seed[32 + i] = p[i];

        std::vector<uint8_t> seed_hash(64, 0);
        keccak_512(&seed_hash[0], 64, &seed[0], 40);

        uint32_t seed_head = seed_hash[0] | (seed_hash[1] << 8) | (seed_hash[2] << 16) | (seed_hash[3] << 24);
        std::vector<uint32_t> mix(mixBytes / 4, 0);
        std::vector<uint32_t> temp(mixBytes / 4, 0);
        for (int i = 0; i < mixBytes / 4; i++)
            mix[i] = seed_hash[i % 16 * 4] | (seed_hash[i % 16 * 4 + 1] << 8) | (seed_hash[i % 16 * 4 + 2] << 16) | (seed_hash[i % 16 * 4 + 3] << 24);

        for (int i = 0; i < loopAccesses; i++) {
            uint32_t parent = fnv(uint32_t(i) ^ seed_head, mix[i % (mixBytes / 4)]) % rows;

            for (int j = 0; j < mixBytes / hashBytes; j++) {
                auto data_ptr = lookup(epoch, 2 * parent + j, cache);
                auto data = data_ptr.get();
                for (int k = 0; k < 16; k++)
                    temp[j * 16 + k] = data[k];
            }
            fnvHash(mix, &temp[0], 32);
        }

        for (int i = 0; i < 32; i += 4)
            mix[i / 4] = fnv(fnv(fnv(mix[i], mix[i + 1]), mix[i + 2]), mix[i + 3]);

        hash_256 digest;
        for (int i = 0; i < 8; i++) {
            uint8_t* p = (uint8_t*)&mix[i];
            for (int j = 0; j < 4; j++)
                digest.hash[i * 4 + j] = p[j];
        }

        std::vector<uint8_t> append(64 + 32, 0);
        for (int i = 0; i < 96; i++) {
            if (i < 64)
                append[i] = seed_hash[i];
            else
                append[i] = digest.hash[i - 64];
        }

        hash_256 result;
        keccak_256(&result.hash[0], 32, &append[0], 96);

        return std::make_pair(digest, result);
    }

    hash_256 headerHash(fibos::block_header& header)
    {
        std::string result;
        result.append(RLP::encode(header.parent_hash, encode_hex));
        result.append(RLP::encode(header.uncle_hash, encode_hex));
        result.append(RLP::encode(header.coinbase, encode_hex));
        result.append(RLP::encode(header.state_root_hash, encode_hex));
        result.append(RLP::encode(header.transaction_root_hash, encode_hex));
        result.append(RLP::encode(header.receipt_root_hash, encode_hex));
        result.append(RLP::encode(header.blooms, encode_hex));
        result.append(RLP::encode(header.difficulty, encode_hex));
        result.append(RLP::encode(header.number, encode_hex));
        result.append(RLP::encode(header.gas_limit, encode_hex));
        result.append(RLP::encode(header.gas_used, encode_hex));
        result.append(RLP::encode(header.timestamp, encode_hex));
        result.append(RLP::encode(header.extra_hash, encode_hex));
        result.insert(0, RLP::generate_array_header(result));

        std::vector<uint8_t> h(result.begin(), result.end());
        uint8_t* in = &h[0];
        hash_256 sharesult;
        keccak_256(&(sharesult.hash[0]), 32, in, h.size());
        return sharesult;
    }

    bool verifyBlockHeader(fibos::block_header& header)
    {
        uint64_t block_number = std::stoul(header.number, nullptr, 16);
        uint64_t epoch = block_number / BLOCKS_PER_EPOCH;
        uint32_t* cache = NULL;
        {
            std::lock_guard<std::mutex> lock(epoch_mutex);
            if ((lower_epoch <= epoch) && (lower_epoch + cache_vec.size() > epoch))
                cache = cache_vec[epoch - lower_epoch].get();
        }
        if (!cache)
            return false;

        hash_256 hash_without_nonce = headerHash(header);

        uint64_t nonce = std::stoul(header.nonce, nullptr, 16);
        hash_256 mix_digest = hex_to_sha256(header.mix_hash);

        std::pair<hash_256, hash_256> result = hashimoto(epoch, cache, hash_without_nonce, nonce);
        hash_256 digest = result.first;
        hash_256 pow = result.second;
        if (!(digest == mix_digest))
            return false;

        const BaseConverter& hex2dec = BaseConverter::HexToDecimalConverter();
        InfInt big256(hex2dec.Convert("ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"));
        InfInt diff(hex2dec.Convert(header.difficulty));

        InfInt pow_int(hex2dec.Convert(sha256_to_hex(pow)));
        InfInt target = big256 / diff;
        if (pow_int > target)
            return false;

        return true;
    }

    bool verifyBlockHeader_v2(fibos::nonce_header& header)
    {
        uint64_t block_number = std::stoul(header.number, nullptr, 16);
        uint64_t epoch = block_number / BLOCKS_PER_EPOCH;
        uint32_t* cache = NULL;
        {
            std::lock_guard<std::mutex> lock(epoch_mutex);
            if ((lower_epoch <= epoch) && (lower_epoch + cache_vec.size() > epoch))
                cache = cache_vec[epoch - lower_epoch].get();
        }
        if (!cache)
            return false;

        hash_256 hash_without_nonce = hex_to_sha256(header.hash_without_nonce);

        uint64_t nonce = std::stoul(header.nonce, nullptr, 16);
        hash_256 mix_digest = hex_to_sha256(header.mix_hash);

        std::pair<hash_256, hash_256> result = hashimoto(epoch, cache, hash_without_nonce, nonce);
        hash_256 digest = result.first;
        hash_256 pow = result.second;
        if (!(digest == mix_digest))
            return false;

        const BaseConverter& hex2dec = BaseConverter::HexToDecimalConverter();
        InfInt big256(hex2dec.Convert("ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"));
        InfInt diff(hex2dec.Convert(header.difficulty));

        InfInt pow_int(hex2dec.Convert(sha256_to_hex(pow)));
        InfInt target = big256 / diff;
        if (pow_int > target)
            return false;

        return true;
    }

    bool isInByteArray(uint8_t* arr, size_t count, size_t pos)
    {
        if (pos >= count * 8)
            return false;
        return ((arr[pos / 8] >> pos % 8) & 0x01);
    }

    void aggregateBlsKey(std::vector<std::vector<uint8_t>> keys, std::vector<uint8_t> bit_mask, const unsigned char* buf) {
        size_t mask_size = bit_mask.size();
        blst_p1 point;
        blst_p1_affine pk;
        bool is_init = false;
        for (size_t i = 0; i < keys.size(); i++) {
            if (!isInByteArray(&bit_mask[0], mask_size, i)) {
                continue;
            }
            blst_p1_uncompress(&pk, keys[i].data());
            if (!is_init) {
                blst_p1_from_affine(&point, &pk);
                is_init = true;
            } else {
                blst_p1_add_or_double_affine(&point, &point, &pk);
            }
        }
        blst_p1_compress((byte*)buf, &point);
    }

    void register_plugin_map()
    {
        auto invoke_init_cache = [=](const apply_context& context, legacy_span<char> data, legacy_span<char> ret) {
            EOS_ASSERT(context.get_action().account == chain::config::eosio_cross_name, wasm_execution_error, "only cross contract can execute this function!");

            uint64_t verify_number;
            fc::raw::unpack<uint64_t>(data.data(), data.size(), verify_number);
            uint64_t epoch = verify_number / BLOCKS_PER_EPOCH;
            EOS_ASSERT(epoch, wasm_execution_error, "invalid epoch number!");
            start_epoch(epoch + epoch_vec_size - 1);
            return 0;
        };
        global_var::instance().add_callback(eth_key, "init_eth_epoch", invoke_init_cache);

        auto invoke_verify_header = [=](const apply_context& context, legacy_span<char> data, legacy_span<char> ret) {
            EOS_ASSERT(context.get_action().account == chain::config::eosio_cross_name, wasm_execution_error, "only cross contract can execute this function!");
            fibos::block_header header;
            fc::raw::unpack<fibos::block_header>(data.data(), data.size(), header);
            bool result = verifyBlockHeader(header);
            uint64_t block_number = std::stoul(header.number, nullptr, 16);
            uint64_t current_epoch = block_number / BLOCKS_PER_EPOCH;
            uint64_t epoch = current_epoch + (epoch_vec_size - update_epoch_pos);

            start_epoch(epoch);

            auto ret_char = fc::raw::pack(result);
            EOS_ASSERT(ret_char.size() <= ret.size(), wasm_execution_error, "ret_size is too small!");
            memcpy(ret.data(), &ret_char[0], ret_char.size());
            return ret_char.size();
        };
        global_var::instance().add_callback(eth_key, "verify_header", invoke_verify_header);

        auto invoke_verify_header_v2 = [=](const apply_context& context, legacy_span<char> data, legacy_span<char> ret) {
            EOS_ASSERT(context.get_action().account == chain::config::eosio_cross_name, wasm_execution_error, "only cross contract can execute this function!");
            fibos::nonce_header header;
            fc::raw::unpack<fibos::nonce_header>(data.data(), data.size(), header);

            //
            bool result = verifyBlockHeader_v2(header);
            uint64_t block_number = std::stoul(header.number, nullptr, 16);
            uint64_t current_epoch = block_number / BLOCKS_PER_EPOCH;
            uint64_t epoch = current_epoch + (epoch_vec_size - update_epoch_pos);
            start_epoch(epoch);

            auto ret_char = fc::raw::pack(result);
            EOS_ASSERT(ret_char.size() <= ret.size(), wasm_execution_error, "ret_size is too small!");
            memcpy(ret.data(), &ret_char[0], ret_char.size());
            return ret_char.size();
        };
        global_var::instance().add_callback(eth_key, "verify_header_v2", invoke_verify_header_v2);

        auto convert_eth_public = [=](const apply_context& context, legacy_span<char> data, legacy_span<char> ret) {
            EOS_ASSERT(context.get_action().account == chain::config::eosio_cross_name, wasm_execution_error, "only cross contract can execute this function!");
            std::vector<public_key_type> packed_keys;
            fc::raw::unpack<std::vector<public_key_type>>(data.data(), data.size(), packed_keys);
            std::vector<fibos::split_public_key_data> unpacked_keys;
            for (public_key_type pub_key : packed_keys) {
                auto unpack_key = fc::crypto::serialize_ecc_point(pub_key);
                split_public_key_data split_key;
                memcpy(split_key.data, unpack_key.begin() + 1, unpack_key.size() - 1);
                unpacked_keys.emplace_back(split_key);
            }
            auto ret_char = fc::raw::pack(unpacked_keys);
            EOS_ASSERT(ret_char.size() <= ret.size(), wasm_execution_error, "ret_size is too small!");
            memcpy(ret.data(), &ret_char[0], ret_char.size());
            return ret_char.size();
        };
        global_var::instance().add_callback(eth_key, "convert_eth_public", convert_eth_public);

        auto aggregate_bls_key = [=](const chain::apply_context& context, chain::legacy_span<char> data, chain::legacy_span<char> ret) {
            EOS_ASSERT(context.get_action().account == chain::config::eosio_cross_name, wasm_execution_error, "only cross contract can execute this function!");

            aggregate_key_data agg_data;
            fc::raw::unpack<aggregate_key_data>(data.data(), data.size(), agg_data);

            unsigned char buf[48] = {0};
            aggregateBlsKey(agg_data.keys, agg_data.mask, buf);
            memcpy(ret.data(), buf, 48);
            return ret.size();
        };
        chain::global_var::instance().add_callback(cross_key, "aggregate_bls_key", aggregate_bls_key);

        auto verify_bls_signature = [=](const chain::apply_context& context, chain::legacy_span<char> data, chain::legacy_span<char> ret) {
            EOS_ASSERT(context.get_action().account == chain::config::eosio_cross_name, wasm_execution_error, "only cross contract can execute this function!");

            verify_signature_data check_data;
            fc::raw::unpack<verify_signature_data>(data.data(), data.size(), check_data);

            blst_p1_affine pk;
            blst_p1_uncompress(&pk, &(check_data.pub_key[0]));

            blst_p2_affine sig;
            blst_p2_uncompress(&sig, &(check_data.signature[0]));
            const unsigned char DST_G1_POP[] = "BLS_SIG_BLS12381G2_XMD:SHA-256_SSWU_RO_POP_";

            bool res = !blst_core_verify_pk_in_g1(&pk, &sig, 1, &(check_data.data[0]), check_data.data.size(),
                DST_G1_POP, sizeof(DST_G1_POP) - 1, NULL, 0);

            memcpy(ret.data(), &res, sizeof(res));
            return sizeof(res);
        };
        chain::global_var::instance().add_callback(cross_key, "verify_bls_signature", verify_bls_signature);

        try {
            auto epoch_pair = get_db_epoch();
            if (epoch_pair.first)
                generate_epoch(epoch_pair.second + epoch_vec_size - 1);
        } catch (...) {
            elog("Get db epoch failed!");
        }
    }

    void start_epoch(uint64_t epoch)
    {
        uint64_t new_lower_epoch = epoch - epoch_vec_size + 1;
        auto& chain = chain_plug->chain();
        chainbase::database& db = const_cast<chainbase::database&>(chain.db());

        const auto& idx = db.get_index<ethash_block_number_index, by_id>();
        if (idx.begin() == idx.end()) {
            db.create<ethash_block_number_object>([&](auto& ebn) {
                ebn.epoch = new_lower_epoch;
            });
        } else {
            if (idx.begin()->epoch != new_lower_epoch) {
                db.modify<ethash_block_number_object>(*(idx.begin()), [&](auto& ebn) {
                    ebn.epoch = new_lower_epoch;
                });
            }
        }

        std::lock_guard<std::mutex> lock(epoch_mutex);
        if (!lower_epoch)
            lower_epoch = new_lower_epoch;
        if ((!is_generating) && (lower_epoch + cache_vec.size() <= epoch)) {
            is_generating = true;
            auto gencache_thread = std::thread([=] { generate_epoch(epoch); });
            gencache_thread.detach();
        }
    }

    void generate_epoch(uint64_t epoch)
    {
        std::unique_lock<std::mutex> lock(epoch_mutex);
        if (!lower_epoch)
            lower_epoch = epoch - epoch_vec_size + 1;
        while (lower_epoch + cache_vec.size() <= epoch) {
            uint64_t current_epoch = lower_epoch + cache_vec.size();
            lock.unlock();

            auto result = gencache(current_epoch * BLOCKS_PER_EPOCH);
            lock.lock();
            cache_vec.emplace_back(result);
            if (cache_vec.size() > epoch_vec_size) {
                cache_vec.erase(cache_vec.begin());
                lower_epoch += 1;
            }
        }
        is_generating = false;
    }
};

cross_plugin::cross_plugin()
    : my(new cross_plugin_impl(app().get_io_service()))
{
}

cross_plugin::~cross_plugin()
{
}

void cross_plugin::set_program_options(
    boost::program_options::options_description& command_line_options,
    boost::program_options::options_description& config_file_options)
{
    auto default_priv_key = private_key_type::regenerate<fc::ecc::private_key_shim>(fc::sha256::hash(std::string("nathan")));

    boost::program_options::options_description bp_signature_options;
    bp_signature_options.add_options()("signature-producer", bpo::value<std::string>()->default_value("eosio"),
        "producer name that sign inter block chain data")("signature-private-key",  bpo::value<std::string>()->default_value(default_priv_key.to_string([]() {})),
        "producer private key that sign inter block chain data");

    config_file_options.add(bp_signature_options);
}


template <typename T>
T dejsonify(const string& s)
{
    return fc::json::from_string(s).as<T>();
}

void cross_plugin::plugin_initialize(const variables_map& options)
{
    try {
        my->chain_plug = app().find_plugin<chain_plugin>();
        EOS_ASSERT(my->chain_plug, chain::missing_chain_plugin_exception, "");
        my->_options = &options;
        auto& chain = my->chain_plug->chain();
        chainbase::database& db = const_cast<chainbase::database&>(chain.db());
        db.add_index<ethash_block_number_index>();
        my->register_plugin_map();

        my->_producer = string_to_name(options.at("signature-producer").as<std::string>());

        my->_sign_private_key = private_key_type(options.at("signature-private-key").as<std::string>());
        my->_sign_public_key = my->_sign_private_key.get_public_key();

        if (options.count("private-key")) {
            const std::vector<std::string> pub_pri_pairs = options["private-key"].as<std::vector<std::string>>();
            for (const std::string& pub_pri_str : pub_pri_pairs) {
                try {
                    auto pub_pri_pair = dejsonify<std::pair<public_key_type, private_key_type>>(pub_pri_str);
                    my->_signature_providers[pub_pri_pair.first] = pub_pri_pair.second;
                } catch (fc::exception& e) {
                    elog("Malformed private key pair");
                }
            }
        }

        if (options.count("signature-provider")) {
            const std::vector<std::string> pub_pri_pairs = options["signature-provider"].as<std::vector<std::string>>();
            for (const auto& pub_pri_str : pub_pri_pairs) {
                try {
                    auto delim = pub_pri_str.find("=");
                    EOS_ASSERT(delim != std::string::npos, plugin_config_exception, "Missing \"=\" in the key spec pair");
                    auto pub_key_str = pub_pri_str.substr(0, delim);
                    auto spec_str = pub_pri_str.substr(delim + 1);

                    auto spec_delim = spec_str.find(":");
                    EOS_ASSERT(spec_delim != std::string::npos, plugin_config_exception, "Missing \":\" in the key spec pair");
                    auto spec_type_str = spec_str.substr(0, spec_delim);
                    auto spec_data = spec_str.substr(spec_delim + 1);

                    auto pubkey = public_key_type(pub_key_str);

                    if (spec_type_str == "KEY")
                        my->_signature_providers[pubkey] = private_key_type(spec_data);
                } catch (...) {
                    elog("Malformed signature provider: \"${val}\", ignoring!", ("val", pub_pri_pairs));
                }
            }
        }
    }
    FC_LOG_AND_RETHROW()
}

#define CALL_WITH_400(api_name, api_handle, call_name, INVOKE, http_response_code)        \
    {                                                                                     \
        std::string("/v1/" #api_name "/" #call_name), api_category::chain_ro,             \
            [api_handle](string&&, string&& body, url_response_callback&& cb) mutable {   \
                try {                                                                     \
                    body = parse_params<std::string, http_params_types::no_params>(body); \
                    INVOKE                                                                \
                    cb(http_response_code, fc::variant(result));                          \
                } catch (...) {                                                           \
                    http_plugin::handle_exception(#api_name, #call_name, body, cb);       \
                }                                                                         \
            }                                                                             \
    }

#define INVOKE_R_V(api_handle, call_name) \
    auto result = api_handle->call_name();

void cross_plugin::plugin_startup()
{
    my->handle_action_loop();
    app().get_plugin<http_plugin>().add_api(
        {
            CALL_WITH_400(ethash, this, get_epoch,
                INVOKE_R_V(this, get_epoch), 200),
            CALL_WITH_400(bpsign, this, get_sign_key,
                INVOKE_R_V(this, get_sign_key), 200),
        },
        appbase::exec_queue::read_only, appbase::priority::low);
}

void cross_plugin::plugin_shutdown()
{
    try {
        ilog("exit shutdown");
        my->_timer.cancel();
    } catch (fc::exception& e) {
        edump((e.to_detail_string()));
    }
}

get_epoch_result cross_plugin::get_epoch() const
{
    get_epoch_result res = { my->get_db_epoch().second, my->lower_epoch, my->cache_vec.size() };
    return res;
}

get_sign_key_result cross_plugin::get_sign_key() const
{
    get_sign_key_result res = { my->_producer, my->_sign_public_key };
    return res;
}

#undef INVOKE_R_V
#undef CALL
} // namespace eosio

FC_REFLECT(eosio::signature_info, (signee)(signature))