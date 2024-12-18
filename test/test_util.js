let _pubkey = 'PUB6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV';
let _prvkey = '5KQwrPbwdL6PhXujxW37FSSQZ1JiwsST4cqQzDeyXtP79zkvFD3';

let FIBOS = require('@tpblock/client')
let child_process = require('child_process');
let http = require('http');
let coroutine = require('coroutine');
let path = require('path');
var solc = require('solc');

const base32_chars = 'abcdefghijklmnopqrstuvwxyz12345';

function new_name() {
    let name = 'test';
    for (let i = 0; i < 8; i++)
        name += base32_chars.substr(Math.floor(Math.random() * 31), 1);
    return name;
}

exports.name = "fibos";
exports.new_name = new_name;

exports.user = (fibos, name, pubkey) => {
    name = name || new_name();
    if (!pubkey)
        pubkey = _pubkey;
    if (!Array.isArray(pubkey))
        pubkey = [pubkey];

    pubkey.sort();
    pubkey = pubkey.map((key) => {
        return {
            "key": key,
            "weight": 1
        };
    });

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
                    keys: pubkey,
                    accounts: [],
                    waits: []
                },
                active: {
                    threshold: 1,
                    keys: pubkey,
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
            // {
            //     account: exports.name,
            //     name: 'delegatebw',
            //     authorization: [{
            //         actor: exports.name,
            //         permission: 'active',
            //     }],
            //     data: {
            //         payer: exports.name,
            //         receiver: name,
            //         bytes: 8192,
            //     },
            // }
        ]
    }, {
        blocksBehind: 3,
        expireSeconds: 30,
    });

    return name;
}

exports.compileContract = function (contractCode) {
    var input = {
        language: 'Solidity',
        sources: {},
        settings: {
            outputSelection: {
                '*': {
                    '*': ['*']
                }
            }
        }
    };

    const contractName = "test.sol";

    input.sources[contractName] = {
        content: contractCode
    }

    var res = JSON.parse(solc.compile(JSON.stringify(input)));

    if (res.errors)
        res.errors.forEach(e => {
            if (e.type != 'Warning')
                throw new Error(e.formattedMessage);
        });

    var contracts = res.contracts[contractName];

    var results = {};
    for (var n in contracts) {
        var contract = contracts[n];
        results[n] = {
            code: Buffer.from('65766d00' + contract.evm.bytecode.object, 'hex'),
            raw_code: Buffer.from(contract.evm.bytecode.object, 'hex'),
            abi: contract.abi
        };
    };

    return results;
};

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

    p = child_process.fork(path.resolve(__dirname, './node/chain.js'), [exports.name], {
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
    keyProvider: _prvkey, // WIF string or array of keys..
    httpEndpoint: 'http://127.0.0.1:8870'
};