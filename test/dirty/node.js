var fibos = require('chain');
var path = require('path');

fibos.config_dir = path.join(__dirname, "./data");
fibos.data_dir = path.join(__dirname, "./data");

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

fibos.load("chain", {
    "delete-all-blocks": true,
    "contracts-console": true
});

if(fibos.is_dirty()){
    // hard-replay-blockchain or others
}

fibos.start();