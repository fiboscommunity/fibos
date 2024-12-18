/* eslint-disable import/no-unresolved */
/* eslint-disable global-require */
// eslint-disable-next-line import/no-unresolved
const chain = require("chain");
const config = require("./config.json");
const path = require("path")
const fs = require("fs")
var _prvkey = '5KQwrPbwdL6PhXujxW37FSSQZ1JiwsST4cqQzDeyXtP79zkvFD3';

function cleanup_folder(p) {
    var dir = fs.readdir(p);
    console.log("clean", p);
    dir.forEach(function (name) {
        var fname = path.join(p, name);
        var f = fs.stat(fname);
        if (f.isDirectory())
            cleanup_folder(fname);
        else
            fs.unlink(fname);
    });

    fs.rmdir(p);
}

chain.name = process.argv[2] || "eosio";
chain.log_level = 'all';

chain.config_dir = chain.data_dir = path.join(__dirname, config.node_dir);

chain.load("http", {
    "http-server-address": `0.0.0.0:${config.http_port}`,
    "access-control-allow-origin": "*",
    "http-validate-host": false,
    "verbose-http-errors": true,
});

chain.load("net", {
    "p2p-listen-endpoint": `0.0.0.0:${config.p2p_port}`,
    "p2p-peer-address": config.p2p_peer_addres,
});

const chain_config = {
    "contracts-console": config.contracts_console,
    "transaction-finality-status-max-storage-size-gb": 4
};

if (config.delete_all_blocks) chain_config["delete-all-blocks"] = config.delete_all_blocks;
if (config.genesis_json) {
    fs.writeFile(path.join(__dirname, config.genesis_json), JSON.stringify({
        "initial_timestamp": new Date().toISOString().substr(0, 19),
        "initial_key": "PUB6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV",
        "initial_configuration": {
            "max_block_net_usage": 524288,
            "target_block_net_usage_pct": 1000,
            "max_transaction_net_usage": 524287,
            "base_per_transaction_net_usage": 12,
            "net_usage_leeway": 500,
            "context_free_discount_net_usage_num": 20,
            "context_free_discount_net_usage_den": 100,
            "max_block_cpu_usage": 400000,
            "target_block_cpu_usage_pct": 10,
            "max_transaction_cpu_usage": 150000,
            "min_transaction_cpu_usage": 1,
            "max_transaction_lifetime": 3600,
            "deferred_trx_expiration_window": 600,
            "max_transaction_delay": 3888000,
            "max_inline_action_size": 524287,
            "max_inline_action_depth": 32,
            "max_authority_depth": 6,
            "max_ram_size": 34359738368
        }
    }, null, '  '));
    chain_config["genesis-json"] = path.join(__dirname, config.genesis_json);
}

chain.load("producer", {
    'producer-name': chain.name,
    'enable-stale-production': true,
    "read-only-threads": 3
});
chain.load("chain", chain_config);

chain.load("resource_monitor", {
    'resource-monitor-not-shutdown-on-threshold-exceeded': true
});

chain.load("chain_api");
chain.load("emitter");

chain.on("close", () => cleanup_folder(chain.data_dir));

chain.start();
