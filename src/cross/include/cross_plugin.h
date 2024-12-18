/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */
#pragma once
#include <eosio/chain/application.hpp>
#include <eosio/chain_plugin/chain_plugin.hpp>
#include <eosio/http_plugin/http_plugin.hpp>

namespace eosio {
using namespace appbase;

struct get_epoch_result {
    uint64_t lower_epoch;
    uint64_t memory_lower_epoch;
    uint64_t epoch_size;
};

struct verify_signature_data {
    std::vector<uint8_t> pub_key;
    std::vector<uint8_t> signature;
    std::vector<uint8_t> data;
};

struct aggregate_key_data {
    std::vector<std::vector<uint8_t>> keys;
    std::vector<uint8_t> mask;
};

struct get_sign_key_result {
    account_name producer;
    public_key_type public_key;
};

class cross_plugin : public appbase::plugin<cross_plugin> {
public:
    cross_plugin();
    virtual ~cross_plugin();

    APPBASE_PLUGIN_REQUIRES((http_plugin)(chain_plugin))
    virtual void set_program_options(
        boost::program_options::options_description& command_line_options,
        boost::program_options::options_description& config_file_options) override;

    void plugin_initialize(const variables_map& options);
    void plugin_startup();
    void plugin_shutdown();

    get_epoch_result get_epoch() const;
    get_sign_key_result get_sign_key() const;

private:
    std::unique_ptr<class cross_plugin_impl> my;
};
}

FC_REFLECT(eosio::verify_signature_data, (pub_key)(signature)(data))
FC_REFLECT(eosio::aggregate_key_data, (keys)(mask))
FC_REFLECT(eosio::get_epoch_result, (lower_epoch)(memory_lower_epoch)(epoch_size))
FC_REFLECT(eosio::get_sign_key_result, (producer)(public_key))
