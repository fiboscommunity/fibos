let child_process = require('child_process');
let http = require('http');
let coroutine = require('coroutine');
let path = require('path');

exports.name = "fibos";

let p;

function stop_node() {
    if (p) {
        p.off("exit");
        p.kill("SIGINT");
    }
    p = null;
}

exports.node = () => {
    if (p) return;
    p = child_process.fork(path.resolve(__dirname, './node.js'), [exports.name], {
        stdio: "ignore"
        // stdio: "inherit"
    });

    p.on("exit", () => {
        console.error("node not executed.");
        process.exit(-1);
    })

    while (true) {
        coroutine.sleep(100);
        try {
            var info = http.get(exports.config.httpEndpoint + "/v1/chain/get_info").json();
            exports.config.chainId = info.chain_id;
            if (info.head_block_num > 3)
                break;
        } catch (e) { }
    }
}

exports.stop = stop_node;

exports.config = {
    httpEndpoint: 'http://127.0.0.1:8871',
    chainSvrEndpoint: 'http://127.0.0.1:8889/post'
};