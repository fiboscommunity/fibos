#pragma once

namespace eosio {
namespace chain {
    enum {
        event_cache_object_type = 10000, ///< Defined by emitter_plugin
        ethash_block_number_object_type, ///< Defined by ethash_plugin
    };
}
} // eosio::chain