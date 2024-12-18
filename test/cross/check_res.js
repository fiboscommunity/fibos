var test = require('test');
test.setup();

var fs = require('fs');
var path = require('path');
var FIBOS = require('@tpblock/client')
var test_util = require('./test_util');
var encoding = require('encoding');
var uuid = require('uuid');

describe('basic ', () => {
    var name = "eosio.cross";
    let producer_name = test_util.producers[0].producer_name;
    let cpu_used = 0;
    let net_used = 0;
    var fibos;
    let zero_res_accout;

    before(() => {
        require.main === module && test_util.node();
        fibos = FIBOS(test_util.config);
    });
    after(() => {
        require.main === module && test_util.stop();
    });

    /**
     * @code 
     * 
     * [[eosio::action]] void hello::sign(const std::vector<char> &binary_data, int times)
     * {
     *     checksum256 result;
     *     check(times >= 0, "times must be positive");
     *     auto input_bytes = reinterpret_cast<const char *>(binary_data.data());
     *     auto input_size = binary_data.size();
     *     result = sha256(input_bytes, input_size);
     *     for (int i = 0; i < times; i++)
     *     {
     *         result = sha256(reinterpret_cast<const char *>(&result), sizeof(result));
     *     }
     * }
     * 
     */
    it('cross setwasm', () => {
        fibos.transact_sync({
            actions: [{
                account: "eosio",
                name: 'setcode',
                authorization: [
                    {
                        actor: name,
                        permission: 'active',
                    },
                ],
                data: {
                    account: name,
                    vmtype: '0',
                    vmversion: '0',
                    code: fs.readFile(path.resolve(__dirname, './contracts/sign.wasm')).hex(),
                },
            }]
        }, {
            blocksBehind: 3,
            expireSeconds: 30,
        });
    });

    it('cross setabi', () => {
        let abi = JSON.parse(fs.readFile(path.resolve(__dirname, './contracts/sign.abi')));
        var _abi = fibos.jsonToRawAbi(abi);
        fibos.transact_sync({
            actions: [{
                account: "eosio",
                name: 'setabi',
                authorization: [
                    {
                        actor: name,
                        permission: 'active',
                    },
                ],
                data: {
                    account: name,
                    abi: encoding.hex.encode(_abi)
                },
            }]
        }, {
            blocksBehind: 3,
            expireSeconds: 30,
        });
    });

    describe(`zero resources but not producer`, () => {
        it(`create a zero_res_accout`, () => {
            let zero_res_accout = test_util.user(fibos);
            let r = fibos.rpc.get_account_sync(zero_res_accout);
            assert.equal(r.cpu_limit.available, 0);
            assert.equal(r.net_limit.available, 0);
        });

        it(`sign`, () => {
            assert.throws(() => {
                fibos.transact_sync({
                    actions: [{
                        account: name,
                        name: 'sign',
                        authorization: [
                            {
                                actor: zero_res_accout,
                                permission: 'active',
                            },
                        ],
                        "data": {
                            "binary_data": new Buffer("hello").toString('hex'),
                            "times": 1
                        }
                    }]
                }, {
                    blocksBehind: 3,
                    expireSeconds: 30,
                });
            })
        });
    });

    describe('enough resources, but not expended', () => {
        it(`get account res`, () => {
            let r = fibos.rpc.get_account_sync(producer_name);
            cpu_used = r.cpu_limit.used;
            net_used = r.net_limit.used;
        })

        it(`sign`, () => {
            fibos.transact_sync({
                actions: [{
                    account: name,
                    name: 'sign',
                    authorization: [
                        {
                            actor: producer_name,
                            permission: 'active',
                        },
                    ],
                    "data": {
                        "binary_data": new Buffer("hello").toString('hex'),
                        "times": 1
                    }
                }]
            }, {
                blocksBehind: 3,
                expireSeconds: 30,
            });
        })

        it(`check account res`, () => {
            let r = fibos.rpc.get_account_sync(producer_name);
            assert.equal(r.cpu_limit.used, cpu_used);
            assert.equal(r.net_limit.used, net_used);
        })
    });

    describe(`not enough resources`, () => {
        it(`undelegatebw resources`, () => {
            fibos.transact_sync({
                actions: [{
                    account: "eosio",
                    name: 'undelegatebw',
                    authorization: [
                        {
                            actor: producer_name,
                            permission: 'active',
                        },
                    ],
                    data: {
                        from: producer_name,
                        receiver: producer_name,
                        unstake_net_quantity: '99999995.0000 SYS',
                        unstake_cpu_quantity: '99999990.0000 SYS',
                    },
                }]
            }, {
                blocksBehind: 3,
                expireSeconds: 30,
            });
        });

        it(`depletion resources`, () => {
            assert.throws(() => {
                while (true) {
                    fibos.transact_sync({
                        actions: [{
                            account: name,
                            name: 'depletion',
                            authorization: [
                                {
                                    actor: producer_name,
                                    permission: 'active',
                                },
                            ],
                            "data": {
                                "binary_data": new Buffer(800).fill(uuid.random()).toString("hex"),
                                "times": 220
                            }
                        }]
                    }, {
                        blocksBehind: 3,
                        expireSeconds: 30,
                    });
                }
            });
            let r = fibos.rpc.get_account_sync(producer_name);
            cpu_used = r.cpu_limit.used;
            net_used = r.net_limit.used;
        })

        it(`sign test cpu`, () => {
            fibos.transact_sync({
                actions: [{
                    account: name,
                    name: 'sign',
                    authorization: [
                        {
                            actor: producer_name,
                            permission: 'active',
                        },
                    ],
                    "data": {
                        "binary_data": new Buffer("hello").toString('hex'),
                        // it will loop 1000 times
                        "times": 1000
                    }
                }]
            }, {
                blocksBehind: 3,
                expireSeconds: 30,
            });
            let r = fibos.rpc.get_account_sync(producer_name);
            assert.equal(r.cpu_limit.used, cpu_used);
            assert.equal(r.net_limit.used, net_used);
        });

        it(`sign test net`, () => {
            fibos.transact_sync({
                actions: [{
                    account: name,
                    name: 'sign',
                    authorization: [
                        {
                            actor: producer_name,
                            permission: 'active',
                        },
                    ],
                    "data": {
                        // create 500kb data
                        "binary_data": new Buffer(new Array(500 * 1024).fill(0)).toString('hex'),
                        "times": 100
                    }
                }]
            }, {
                blocksBehind: 3,
                expireSeconds: 30,
            });
            let r = fibos.rpc.get_account_sync(producer_name);
            assert.equal(r.cpu_limit.used, cpu_used);
            assert.equal(r.net_limit.used, net_used);
        });
    });
});

require.main === module && test.run(console.DEBUG);