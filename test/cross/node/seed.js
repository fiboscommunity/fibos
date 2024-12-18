var chain = require('chain');
let config = require('./config.json');
let path = require('path');

chain.config_dir = path.resolve(__dirname, config.node_dir);
chain.data_dir = path.resolve(__dirname, config.node_dir);

console.notice("config_dir:", chain.config_dir);
console.notice("data_dir:", chain.data_dir);

chain.load("http", {
    "http-server-address": `0.0.0.0:${config.http_port}`,
    "access-control-allow-origin": "*",
    "http-validate-host": false,
    "verbose-http-errors": true,
    "http-max-response-time-ms": 3000
});

chain.load("net", {
    "p2p-peer-address": `0.0.0.0:${config.p2p_port}`,
    "p2p-listen-endpoint": config.p2p_peer_addres,
});

chain.load("producer", {
    'producer-name': config.producer[0].producer_name,
    'enable-stale-production': true,
    'max-transaction-time': 3000,
    'incoming-transaction-queue-size-mb': 14095,
    'private-key': JSON.stringify([config.producer[0].pub_key, config.producer[0].pri_key])
});


chain.load("chain", {
    "contracts-console": true,
    'chain-state-db-size-mb': 8 * 1024,
    "replay-blockchain": true
});

chain.load("chain_api");
chain.load("trace_api", {
    "trace-no-abis": "true"
});
chain.start();
