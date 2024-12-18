const http = require('http');
var fibos = require('chain');
var path = require('path');

fibos.config_dir = path.join(__dirname, "./data");
fibos.data_dir = path.join(__dirname, "./data");

console.notice("config_dir:", fibos.config_dir);
console.notice("data_dir:", fibos.data_dir);

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
fibos.load("chain_api");

fibos.load("resource_monitor", {
    'resource-monitor-not-shutdown-on-threshold-exceeded': true
});

let svr = new http.Server(8889, [req => {
}, {
    "/post": req => {
        let params = req.json();
        if (params == null || params.url == null) {
            return req.response.json({ code: 100, message: "params error" });
        }
        let url = params.url;
        let data = params.data;
        let res = fibos.post(url, data == null ? null : JSON.stringify(data));
        return req.response.json(JSON.parse(res));
    },
}
])

fibos.on("close", () => {
    svr.stop();
})

svr.start()
fibos.start();