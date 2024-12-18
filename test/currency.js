var test = require('test');
test.setup();

var FIBOS = require('@tpblock/client')
var test_util = require('./test_util');
var encoding = require('encoding');

describe('currency', () => {
    var name;
    var name1;
    var name_token;
    var fibos;

    before(() => {
        require.main === module && test_util.node();
        fibos = FIBOS(test_util.config);

        name = test_util.user(fibos);
        name1 = test_util.user(fibos);
        name_token = test_util.name + ".token";
        try {
            test_util.user(fibos, name_token);
        } catch (e) { }
    });

    after(() => {
        require.main === module && test_util.stop();
    });

    it('get_currency_balance', () => {
        var _abi = fibos.jsonToRawAbi({
            "version": "eosio::abi/1.0",
            "types": [],
            "structs": [{
                "name": "hi",
                "base": "",
                "fields": [{
                    "name": "user",
                    "type": "name"
                }]
            }, {
                "name": "account",
                "base": "",
                "fields": [{
                    "name": "balance",
                    "type": "asset"
                }]
            }],
            "actions": [{
                "name": "hi",
                "type": "hi",
                "ricardian_contract": ""
            }],
            "tables": [{
                "name": "accounts",
                "type": "account",
                "index_type": "i64",
                "key_names": ["currency"],
                "key_types": ["uint64"]
            }
            ],
            "ricardian_clauses": [],
            "error_messages": [],
            "abi_extensions": []
        });

        fibos.transact_sync({
            actions: [{
                account: test_util.name,
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
            }, {
                account: test_util.name,
                name: 'setcode',
                authorization: [
                    {
                        actor: name,
                        permission: 'active'
                    },
                ],
                data: {
                    account: name,
                    vmtype: '0',
                    vmversion: '0',
                    code: Buffer.from(fibos.compileCode(`exports.hi = user => {
                        var accounts = db.accounts(action.account, user);
                        accounts.emplace(user, {currency: 120, balance: "100 SYS"});
                        accounts.emplace(user, {currency: 121, balance: "200.234 EOS"});
                        accounts.emplace(user, {currency: 122, balance: "200.231 FIBOS"});
                    }`)).hex()
                }
            }]
        }, {
            blocksBehind: 3,
            expireSeconds: 30,
        });

        fibos.transact_sync({
            actions: [{
                account: name,
                name: 'hi',
                authorization: [{
                    actor: name1,
                    permission: 'active',
                }],
                "data": {
                    "user": name1
                }
            }]
        }, {
            blocksBehind: 3,
            expireSeconds: 30,
        });

        assert.deepEqual(fibos.rpc.get_currency_balance_sync(name, name1), [
            "100 SYS",
            "200.234 EOS",
            "200.231 FIBOS"
        ]);

        assert.deepEqual(fibos.rpc.get_currency_balance_sync(name, name1, "SYS"), ["100 SYS"]);
        assert.deepEqual(fibos.rpc.get_currency_balance_sync(name, name1, "EOS"), ["200.234 EOS"]);
        assert.deepEqual(fibos.rpc.get_currency_balance_sync(name, name1, "FIBOS"), ["200.231 FIBOS"]);
        assert.deepEqual(fibos.rpc.get_currency_balance_sync(name, name1, "TEST"), []);
    });

    it('get_currency_balance extended', () => {
        var _abi = fibos.jsonToRawAbi({
            "version": "eosio::abi/1.0",
            "types": [],
            "structs": [{
                "name": "hi",
                "base": "",
                "fields": [{
                    "name": "user",
                    "type": "name"
                }]
            }, {
                "name": "extended_asset",
                "base": "",
                "fields": [
                    { "name": "quantity", "type": "asset" },
                    { "name": "contract", "type": "name" }
                ]
            }, {
                "name": "account",
                "base": "",
                "fields": [
                    { "name": "primary", "type": "uint64" },
                    { "name": "balance", "type": "extended_asset" }
                ]
            }],
            "actions": [{
                "name": "hi",
                "type": "hi",
                "ricardian_contract": ""
            }],
            "tables": [{
                "name": "accounts",
                "type": "account",
                "index_type": "i64",
                "key_names": ["primary"],
                "key_types": ["uint64"]
            }
            ],
            "ricardian_clauses": [],
            "error_messages": [],
            "abi_extensions": []
        });

        fibos.transact_sync({
            actions: [{
                account: test_util.name,
                name: 'setabi',
                authorization: [
                    {
                        actor: name1,
                        permission: 'active',
                    },
                ],
                data: {
                    account: name1,
                    abi: encoding.hex.encode(_abi)
                },
            }, {
                account: test_util.name,
                name: 'setcode',
                authorization: [
                    {
                        actor: name1,
                        permission: 'active'
                    },
                ],
                data: {
                    account: name1,
                    vmtype: '0',
                    vmversion: '0',
                    code: Buffer.from(fibos.compileCode(`exports.hi = user => {
                        var accounts = db.accounts(action.account, user);
                        accounts.emplace(user, {primary: 120, balance: {quantity: "100 SYS", contract: action.account}});
                        accounts.emplace(user, {primary: 121, balance: {quantity: "200.234 EOS", contract: "fibos1"}});
                        accounts.emplace(user, {primary: 122, balance: {quantity: "200.231 FIBOS", contract: action.account}});
                    }`)).hex()
                }
            }]
        }, {
            blocksBehind: 3,
            expireSeconds: 30,
        });

        fibos.transact_sync({
            actions: [{
                account: name1,
                name: 'hi',
                authorization: [{
                    actor: name,
                    permission: 'active',
                }],
                "data": {
                    "user": name
                }
            }]
        }, {
            blocksBehind: 3,
            expireSeconds: 30,
        });

        assert.deepEqual(fibos.rpc.get_currency_balance_sync(name1, name), [
            "100 SYS",
            "200.231 FIBOS"
        ]);

        assert.deepEqual(fibos.rpc.get_currency_balance_sync(name1, name, "SYS"), ["100 SYS"]);
        assert.deepEqual(fibos.rpc.get_currency_balance_sync(name1, name, "EOS"), []);
        assert.deepEqual(fibos.rpc.get_currency_balance_sync(name1, name, "FIBOS"), ["200.231 FIBOS"]);
        assert.deepEqual(fibos.rpc.get_currency_balance_sync(name1, name, "TEST"), []);
    });

    it('get_currency_balance extended system token', () => {
        var _abi = fibos.jsonToRawAbi({
            "version": "eosio::abi/1.0",
            "types": [],
            "structs": [{
                "name": "hi",
                "base": "",
                "fields": [{
                    "name": "user",
                    "type": "name"
                }]
            }, {
                "name": "extended_asset",
                "base": "",
                "fields": [
                    { "name": "quantity", "type": "asset" },
                    { "name": "contract", "type": "name" }
                ]
            }, {
                "name": "account",
                "base": "",
                "fields": [
                    { "name": "primary", "type": "uint64" },
                    { "name": "balance", "type": "extended_asset" }
                ]
            }],
            "actions": [{
                "name": "hi",
                "type": "hi",
                "ricardian_contract": ""
            }],
            "tables": [{
                "name": "accounts",
                "type": "account",
                "index_type": "i64",
                "key_names": ["primary"],
                "key_types": ["uint64"]
            }
            ],
            "ricardian_clauses": [],
            "error_messages": [],
            "abi_extensions": []
        });

        fibos.transact_sync({
            actions: [{
                account: test_util.name,
                name: 'setabi',
                authorization: [
                    {
                        actor: name_token,
                        permission: 'active',
                    },
                ],
                data: {
                    account: name_token,
                    abi: encoding.hex.encode(_abi)
                },
            }, {
                account: test_util.name,
                name: 'setcode',
                authorization: [
                    {
                        actor: name_token,
                        permission: 'active'
                    },
                ],
                data: {
                    account: name_token,
                    vmtype: '0',
                    vmversion: '0',
                    code: Buffer.from(fibos.compileCode(`exports.hi = user => {
                        var accounts = db.accounts(action.account, user);
                        accounts.emplace(user, {primary: 120, balance: {quantity: "100 SYS", contract: "${test_util.name}"}});
                        accounts.emplace(user, {primary: 121, balance: {quantity: "200.234 EOS", contract: "fibos1"}});
                        accounts.emplace(user, {primary: 122, balance: {quantity: "200.231 FIBOS", contract: "${test_util.name}"}});
                    }`)).hex()
                }
            }]
        }, {
            blocksBehind: 3,
            expireSeconds: 30,
        });

        fibos.transact_sync({
            actions: [{
                account: name_token,
                name: 'hi',
                authorization: [{
                    actor: name,
                    permission: 'active',
                }],
                "data": {
                    "user": name
                }
            }]
        }, {
            blocksBehind: 3,
            expireSeconds: 30,
        });

        assert.deepEqual(fibos.rpc.get_currency_balance_sync(name_token, name), [
            "100 SYS",
            "200.231 FIBOS"
        ]);

        assert.deepEqual(fibos.rpc.get_currency_balance_sync(name_token, name, "SYS"), ["100 SYS"]);
        assert.deepEqual(fibos.rpc.get_currency_balance_sync(name_token, name, "EOS"), []);
        assert.deepEqual(fibos.rpc.get_currency_balance_sync(name_token, name, "FIBOS"), ["200.231 FIBOS"]);
        assert.deepEqual(fibos.rpc.get_currency_balance_sync(name_token, name, "TEST"), []);
    });
});

require.main === module && test.run(console.DEBUG);