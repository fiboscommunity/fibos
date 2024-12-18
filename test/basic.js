var test = require('test');
test.setup();

var fs = require('fs');
var path = require('path');
var FIBOS = require('@tpblock/client')
var test_util = require('./test_util');
var encoding = require('encoding');

var platform = process.platform;
platform = platform.charAt(0).toUpperCase() + platform.slice(1);
var eos_build_dir = '../eos/build_' + platform + '_Release';

describe('basic', () => {
    var name;
    var fibos;
    var abi = {
        "version": "eosio::abi/1.0",
        "types": [],
        "structs": [{
            "name": "player",
            "base": "",
            "fields": [{
                "name": "title",
                "type": "string"
            }, {
                "name": "age",
                "type": "int64"
            }]
        }, {
            "name": "hi",
            "base": "",
            "fields": [{
                "name": "user",
                "type": "name"
            }]
        }],
        "actions": [{
            "name": "hi",
            "type": "hi",
            "ricardian_contract": ""
        }],
        "tables": [],
        "ricardian_clauses": [],
        "error_messages": [],
        "abi_extensions": []
    };

    before(() => {
        require.main === module && test_util.node();
        fibos = FIBOS(test_util.config);
        name = test_util.user(fibos);
    });
    after(() => {
        require.main === module && test_util.stop();
    });

    it('wasm code', () => {
        fibos.transact_sync({
            actions: [{
                account: test_util.name,
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
                    code: fs.readFile(path.resolve(__dirname, './contracts/hello.wasm')).hex(),
                },
            }]
        }, {
            blocksBehind: 3,
            expireSeconds: 30,
        });
    });

    it('setabi', () => {
        var _abi = fibos.jsonToRawAbi(abi);
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
            }]
        }, {
            blocksBehind: 3,
            expireSeconds: 30,
        });
    });

    it('call contract', () => {
        fibos.transact_sync({
            actions: [{
                account: name,
                name: 'hi',
                authorization: [
                    {
                        actor: name,
                        permission: 'active',
                    },
                ],
                "data": {
                    "user": "lion"
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
                authorization: [
                    {
                        actor: name,
                        permission: 'active',
                    },
                ],
                "data": {
                    "user": "lion1"
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
                authorization: [
                    {
                        actor: name,
                        permission: 'active',
                    },
                ],
                "data": {
                    "user": "lion2"
                }
            }]
        }, {
            blocksBehind: 3,
            expireSeconds: 30,
        });
    })
});

require.main === module && test.run(console.DEBUG);