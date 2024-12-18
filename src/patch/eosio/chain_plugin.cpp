#include "config.hpp"
#include <boost/algorithm/string.hpp>
#include <fc/io/json.hpp>

#define walk_key_value_table eos_walk_key_value_table
#include <eosio/chain_plugin/chain_plugin.hpp>
#undef walk_key_value_table

#define walk_key_value_table(code, scope, table, f) \
    get_currency_result_ex(*this, abi, p, results); \
    return results;

using namespace eosio::chain;

namespace eosio {
namespace chain_apis {
    void get_currency_result_ex(const read_only& ro, const abi_def& abi,
        const read_only::get_currency_balance_params& p, vector<asset>& results)
    {
        bool eos_abi = true;
        for (auto& s : abi.structs) {
            if (s.name == "account") {
                eos_abi = s.fields.size() == 1 && s.fields[0].name == "balance" && s.fields[0].type == "asset";
                break;
            }
        }

        ro.eos_walk_key_value_table(p.code, p.account, "accounts"_n, [&](const key_value_object& obj) {
            EOS_ASSERT(obj.value.size() >= sizeof(asset), asset_type_exception, "Invalid data on table");

            uint64_t key;
            asset cursor;
            name contract;

            fc::datastream<const char*> ds(obj.value.data(), obj.value.size());
            if (eos_abi) {
                fc::raw::unpack(ds, cursor);
            } else {
                fc::raw::unpack(ds, key);
                fc::raw::unpack(ds, cursor);
                fc::raw::unpack(ds, contract);

                if (p.code == config::token_account_name) {
                    if (contract != config::system_account_name) {
                        return true;
                    }
                } else {
                    if (contract != p.code) {
                        return true;
                    }
                }
            }

            EOS_ASSERT(cursor.get_symbol().valid(), asset_type_exception, "Invalid asset");

            if (!p.symbol || boost::iequals(cursor.symbol_name(), *p.symbol)) {
                results.emplace_back(cursor);
            }

            // return false if we are looking for one and found it, true otherwise
            return !(p.symbol && boost::iequals(cursor.symbol_name(), *p.symbol));
        });
    }

    void get_currency_result_ex(const read_only& ro, const abi_def& abi,
        const read_only::get_currency_stats_params& p, fc::mutable_variant_object& results)
    {
        uint64_t scope = (string_to_symbol(0, boost::algorithm::to_upper_copy(p.symbol).c_str()) >> 8);

        ro.eos_walk_key_value_table(p.code, name(scope), "stat"_n, [&](const key_value_object& obj) {
            EOS_ASSERT(obj.value.size() >= sizeof(read_only::get_currency_stats_result), asset_type_exception, "Invalid data on table");

            fc::datastream<const char*> ds(obj.value.data(), obj.value.size());
            read_only::get_currency_stats_result result;

            fc::raw::unpack(ds, result.supply);
            fc::raw::unpack(ds, result.max_supply);
            fc::raw::unpack(ds, result.issuer);

            results[result.supply.symbol_name()] = result;
            return true;
        });
    }
}
}

#include <chain_plugin/chain_plugin.cpp>
