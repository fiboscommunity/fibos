var fibos = require('chain');
var path = require('path');
const snapshot = require('./snapshot');

fibos.config_dir = path.join(__dirname, "./data");
fibos.data_dir = path.join(__dirname, "./data");

let chain_config = {
    "contracts-console": true,
    'chain-state-db-size-mb': 8 * 1024
    // "delete-all-blocks": true
};

fibos.load("http", {
    "http-server-address": "0.0.0.0:8871",
    "access-control-allow-origin": "*",
    "http-validate-host": false,
    "verbose-http-errors": true
});

fibos.load("net", {
    "p2p-peer-address": [
        "127.0.0.1:9870",
    ],
    "p2p-listen-endpoint": "0.0.0.0:9871"
});

fibos.load("producer", {
    'producer-name': 'eosio',
    'enable-stale-production': true,
    'max-transaction-time': 3000
});

fibos.load("chain", chain_config);
fibos.load("chain_api");

snapshot(chain_config);

fibos.on("close", (code) => {
    process.exit(code);
});

fibos.start();