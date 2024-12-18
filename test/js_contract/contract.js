var test = require('test');
test.setup();

var FIBOS = require('@tpblock/client')
var test_util = require('../test_util');
var encoding = require('encoding');

describe('contract', () => {
    var name;
    var name1;
    var fibos;
    var abi = {
        "version": "eosio::abi/1.1",
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
        }, {
            "name": "hi1",
            "base": "",
            "fields": [{
                "name": "user",
                "type": "name"
            }, {
                "name": "friend",
                "type": "name"
            }]
        }],
        "actions": [{
            "name": "hi",
            "type": "hi",
            "ricardian_contract": ""
        }, {
            "name": "hi1",
            "type": "hi1",
            "ricardian_contract": ""
        }],
        "action_results": [{
            "name": "hi1",
            "result_type": "int32"
        }],
        "tables": [],
        "ricardian_clauses": [],
        "variants": [],
        "error_messages": [],
        "abi_extensions": []
    };

    function set_code(code) {
        fibos.transact_sync({
            actions: [{
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
                    code: Buffer.from(code).hex()
                }
            }]
        }, {
            blocksBehind: 3,
            expireSeconds: 30,
            // readOnlyTrx: true
        });
    }

    function set_abi(abi) {
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
                    abi: encoding.hex.encode(fibos.jsonToRawAbi(abi))
                },
            }]
        }, {
            blocksBehind: 3,
            expireSeconds: 30,
        });
    }

    function hi(user) {
        return fibos.transact_sync({
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
                    "user": user
                }
            }]
        }, {
            blocksBehind: 3,
            expireSeconds: 30,
        });
    }

    function hi1(user, friend) {
        return fibos.transact_sync({
            actions: [{
                account: name,
                name: 'hi1',
                authorization: [
                    {
                        actor: name,
                        permission: 'active',
                    },
                ],
                "data": {
                    "user": user,
                    "friend": friend
                }
            }]
        }, {
            blocksBehind: 3,
            expireSeconds: 30,
        });
    }

    before(() => {
        require.main === module && test_util.node();
        fibos = FIBOS(test_util.config);

        name = test_util.user(fibos);
        name1 = test_util.user(fibos);

        set_abi(abi);
    });

    beforeEach(() => {
        fibos = FIBOS(test_util.config);
    });

    after(() => {
        require.main === module && test_util.stop();
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
    });

    describe("params", () => {
        before(() => {
            var js_code = `exports.hi = v => console.error(typeof v, v);
            exports.hi1 = (user, friend) => {console.error(user, friend);return 100}`;
            set_code(fibos.compileCode(js_code));
        });

        it('id', () => {
            var r = hi('lion1234');
            assert.equal(r.processed.action_traces[0].console, 'string lion1234\n');
        });

        it('bool', () => {
            abi.structs[1].fields[0].type = 'bool';
            set_abi(abi);

            var r = hi(1);
            assert.equal(r.processed.action_traces[0].console, 'bigint 1n\n');
        });

        it('int64', () => {
            abi.structs[1].fields[0].type = 'int64';
            set_abi(abi);

            var r = hi(1123);
            assert.equal(r.processed.action_traces[0].console, 'bigint 1123n\n');
        });

        it('uint64', () => {
            abi.structs[1].fields[0].type = 'uint64';
            set_abi(abi);

            assert.throws(() => {
                hi(-1123);
            });

            var r = hi("18446744073709551615");
            assert.equal(r.processed.action_traces[0].console, 'bigint 18446744073709551615n\n');
        });

        it('array', () => {
            abi.structs[1].fields[0].type = 'int64[]';
            set_abi(abi);

            var r = hi([1, 2, 3, 4, 5, 6, 7, 8, 9, 10]);
            assert.equal(r.processed.action_traces[0].console, 'object [\n  1n,\n  2n,\n  3n,\n  4n,\n  5n,\n  6n,\n  7n,\n  8n,\n  9n,\n  10n\n]\n');
        });

        it('object', () => {
            abi.structs[1].fields[0].type = 'player';
            set_abi(abi);

            var r = hi({
                title: "lion",
                age: 123
            });
            assert.equal(r.processed.action_traces[0].console, 'object {\n  \"title\": \"lion\",\n  \"age\": 123n\n}\n');
        });

        it('asset', () => {
            abi.structs[1].fields[0].type = 'asset';
            set_abi(abi);

            var r = hi("10000.0000 SYS");
            assert.equal(r.processed.action_traces[0].console, 'string 10000.0000 SYS\n');
        });

        it('order', () => {
            var r = hi1('lion1', 'lion2');
            assert.equal(r.processed.action_traces[0].console, 'lion1 lion2\n');

            var r = hi1('lion3', 'lion4');
            assert.equal(r.processed.action_traces[0].console, 'lion3 lion4\n');

            var r = hi1('lion', 'lion1');
            assert.equal(r.processed.action_traces[0].console, 'lion lion1\n');
        });

        it('result', () => {
            var r = hi1('lion1', 'lion4');
            assert.equal(r.processed.action_traces[0].return_value_data, 100);
        });

        it('load timeout', () => {
            var js_code = `exports.hi = v => {}; while(true);`;
            set_code(fibos.compileCode(js_code));

            abi.structs[1].fields[0].type = 'name';
            abi.actions[0].ricardian_contract = '111';
            set_abi(abi);

            assert.throws(() => {
                var r = hi("sysys1");
            });
        });

        it('function timeout', () => {
            var js_code = `exports.hi = v => {while(true);}`;
            set_code(fibos.compileCode(js_code));

            assert.throws(() => {
                var r = hi("sysys2");
            });
        });
    });

    it('zip module', () => {
        set_code(fibos.compileModule(__dirname + '/../module'));
        abi.structs[1].fields[0].type = 'name';
        abi.actions[0].ricardian_contract = '112';
        set_abi(abi);

        var r = hi('lion12');
        assert.equal(r.processed.action_traces[0].console, 'lion12  test1 test2\n');
    })

    it('require fs', () => {
        var js_code = `var not_exists = require('${__dirname}/block');
        exports.hi = v => {while(true);}`;
        set_code(fibos.compileCode(js_code));

        var err;

        assert.throws(() => {
            var r = hi("sysys");
        });
    });
});

require.main === module && test.run(console.DEBUG);