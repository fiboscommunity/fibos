

let FIBOS = require('@tpblock/client')
let child_process = require('child_process');
let coroutine = require('coroutine');
let path = require('path');
let fs = require('fs');
let config = require("./node/config.json");
let accounts = config.producer;
let _prvkey = accounts.map(function(d) {
    return d.pri_key;
}).concat(/* name.cross */"5KQwrPbwdL6PhXujxW37FSSQZ1JiwsST4cqQzDeyXtP79zkvFD3")
let _pubkey = config.producer[0].pub_key;
const base32_chars = 'abcdefghijklmnopqrstuvwxyz12345';

function new_name() {
    let name = 'test';
    for (let i = 0; i < 8; i++)
        name += base32_chars.substr(Math.floor(Math.random() * 31), 1);
    return name;
}
exports.producers = config.producer;
exports.name = "eosio";
exports.new_name = new_name;

exports.user = (fibos, name, pubkey) => {
    name = name || new_name();
    pubkey = pubkey || _pubkey;
    fibos.transact_sync({
        actions: [{
            account: exports.name,
            name: 'newaccount',
            authorization: [{
                actor: exports.name,
                permission: 'active',
            }],
            data: {
                creator: exports.name,
                name: name,
                owner: {
                    threshold: 1,
                    keys: [{
                        key: pubkey,
                        weight: 1
                    }],
                    accounts: [],
                    waits: []
                },
                active: {
                    threshold: 1,
                    keys: [{
                        key: pubkey,
                        weight: 1
                    }],
                    accounts: [],
                    waits: []
                }
            },
        },
            // {
            //     account: exports.name,
            //     name: 'buyrambytes',
            //     authorization: [{
            //         actor: exports.name,
            //         permission: 'active',
            //     }],
            //     data: {
            //         from: exports.name,
            //         receiver: name,
            //         stake_net_quantity: '10.0000 SYS',
            //         stake_cpu_quantity: '10.0000 SYS',
            //         transfer: 0
            //     },
            // },
            {
                account: exports.name,
                name: 'buyrambytes',
                authorization: [{
                    actor: exports.name,
                    permission: 'active',
                }],
                data: {
                    payer: exports.name,
                    receiver: name,
                    bytes: 8192,
                },
            }
        ]
    }, {
        blocksBehind: 3,
        expireSeconds: 30,
    });

    return name;
}

let p;

function cleanup_folder(p) {
    var dir = fs.readdir(p);
    dir.forEach(function(name) {
        var fname = path.join(p, name);
        var f = fs.stat(fname);
        if (f.isDirectory()) {
            cleanup_folder(fname);
            fs.rmdir(fname);
        } else
            fs.unlink(fname);
    });
}

function stop_node() {
    if (p) {
        p.off("exit");
        p.kill("SIGINT");
    }
    p = null;
}

exports.node = () => {
    if (p) return;

    if (fs.exists(path.resolve(__dirname, './node/data'))) {
        cleanup_folder(path.resolve(__dirname, './node/data'));
        fs.rmdir(path.resolve(__dirname, './node/data'));
    }

    child_process.exec("tar" + " -xzf " + path.resolve(__dirname, './node/data.tar') + " -C " + path.resolve(__dirname, './node/'));
    coroutine.sleep(2000);
    p = child_process.fork(path.resolve(__dirname, './node/seed.js'), [exports.name], {
        // stdio: "ignore"
        stdio: "inherit"
    });

    p.on("exit", () => {
        console.error("node not executed.");
        process.exit(-1);
    })

    let fibos = FIBOS(exports.config);
    while (true) {
        coroutine.sleep(100);
        try {
            let info = fibos.rpc.get_info_sync();
            exports.config.chainId = info.chain_id;
            if (info.head_block_num > 200)
                break;
        } catch (e) { }
    }
}

exports.stop = stop_node;

exports.config = {
    keyProvider: _prvkey, // WIF string or array of keys..
    httpEndpoint: 'http://127.0.0.1:8870'
};