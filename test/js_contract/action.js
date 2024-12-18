var test = require('test');
test.setup();

var FIBOS = require('@tpblock/client')
var test_util = require('../test_util');
var encoding = require('encoding');

describe('action', () => {
    var name;
    var name1;
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
        "tables": [],
        "ricardian_clauses": [],
        "error_messages": [],
        "abi_extensions": []
    };
    var abi1 = {
        "version": "eosio::abi/1.0",
        "types": [],
        "structs": [],
        "actions": [],
        "tables": [],
        "ricardian_clauses": [],
        "error_messages": [],
        "abi_extensions": []
    };

    function set_code(name, code) {
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
        });
    }

    function set_abi(name, abi) {
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

    function hi(user, auth) {
        return fibos.transact_sync({
            actions: [{
                account: name,
                name: 'hi',
                authorization: [{
                    actor: auth.authorization,
                    permission: 'active',
                }],
                "data": {
                    "user": user
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

        set_abi(name, abi);
        set_abi(name1, abi1);
    });
    after(() => {
        require.main === module && test_util.stop();
    });

    it('name', () => {
        var js_code = `exports.hi = v => console.error(action.name);`;
        set_code(name, fibos.compileCode(js_code));

        var r = hi('lion12', {
            authorization: name1
        });
        assert.equal(r.processed.action_traces[0].console, 'hi\n');
    });

    it('account', () => {
        var js_code = `exports.hi = v => console.error(action.account);`;
        set_code(name, fibos.compileCode(js_code));

        var r = hi('lion1', {
            authorization: name1
        });
        assert.equal(r.processed.action_traces[0].console, name + "\n");
    });

    it('is_account', () => {
        var js_code = `exports.hi = v => console.error(action.is_account(action.account), action.is_account("notexists"));`;
        set_code(name, fibos.compileCode(js_code));

        var r = hi('lion123', {
            authorization: name1
        });
        assert.equal(r.processed.action_traces[0].console, "true false\n");
    });

    it('receiver', () => {
        var js_code = `exports.hi = v => console.error(action.receiver);`;
        set_code(name, fibos.compileCode(js_code));

        var r = hi('lion14', {
            authorization: name1
        });
        assert.equal(r.processed.action_traces[0].console, name + "\n");
    });

    it('has_auth', () => {
        var js_code = `exports.hi = v => console.error(action.has_auth(action.receiver));`;
        set_code(name, fibos.compileCode(js_code));

        var r = hi('lion2', {
            authorization: name1
        });
        assert.equal(r.processed.action_traces[0].console, 'false\n');

        var js_code = `exports.hi = v => console.error(action.has_auth("${name1}"));`;
        set_code(name, fibos.compileCode(js_code));

        var r = hi('lion1222', {
            authorization: name1
        });
        assert.equal(r.processed.action_traces[0].console, 'true\n');
    });

    it('require_auth', () => {
        var js_code = `exports.hi = v => console.error(action.require_auth(action.receiver));`;
        set_code(name, fibos.compileCode(js_code));


        var r = hi('lion4', {
            authorization: name
        });

        assert.throws(() => {
            var r = hi('lion3', {
                authorization: name1
            });
        })
    });

    it('require_auth with permission', () => {
        var js_code = `exports.hi = v =>console.error(action.require_auth(action.receiver, "hello"));`;
        set_code(name, fibos.compileCode(js_code));

        fibos.transact_sync({
            actions: [{
                account: test_util.name,
                name: 'updateauth',
                authorization: [{
                    actor: name,
                    permission: 'active',
                }],
                data: {
                    account: name,
                    permission: 'hello',
                    parent: 'active',
                    auth: {
                        threshold: 1,
                        accounts: [{
                            permission: {
                                actor: name,
                                permission: "active"
                            },
                            weight: 1
                        }],
                        keys: [{
                            key: 'PUB6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV',
                            weight: 1
                        }],
                        waits: []
                    }
                }
            }]
        }, {
            blocksBehind: 3,
            expireSeconds: 30,
        });

        fibos.transact_sync({
            actions: [{
                account: test_util.name,
                name: 'linkauth',
                authorization: [{
                    actor: name,
                    permission: 'active',
                }],
                data: {
                    account: name,
                    code: name,
                    type: 'hi',
                    requirement: 'hello'
                }
            }]
        }, {
            blocksBehind: 3,
            expireSeconds: 30,
        });

        var r = fibos.transact_sync({
            actions: [{
                account: name,
                name: 'hi',
                authorization: [{
                    actor: name,
                    permission: 'hello',
                }],
                "data": {
                    "user": 'lion4'
                }
            }]
        }, {
            blocksBehind: 3,
            expireSeconds: 30,
        });

        assert.throws(() => {
            r = hi('lion3', {
                authorization: name
            });
        })

    });

    it("recipient", () => {
        var js_code = `exports.hi = v => {
                console.error(action.has_recipient(action.receiver), action.has_recipient("${name1}"));
                action.require_recipient("${name1}")
                console.error(action.has_recipient(action.receiver), action.has_recipient("${name1}"));
            }`;
        set_code(name, fibos.compileCode(js_code));

        var js_code1 = `exports.on_hi = v => {
            console.log(action.receiver, action.account , v);
        };
        exports.hi = v => {
            throw new Error();
        }`;
        set_code(name1, fibos.compileCode(js_code1));


        var r = hi('lion412', {
            authorization: name
        });

        assert.equal(r.processed.action_traces[0].console, "true false\ntrue true\n");
        assert.equal(r.processed.action_traces[0].inline_traces[0].console, `${name1} ${name} lion412\n`);
    });

    it("authorization", () => {
        var js_code = `exports.hi = v => {console.error(action.authorization);}`;
        set_code(name, fibos.compileCode(js_code));


        var r = hi('lion413', {
            authorization: name
        });
        assert.equal(r.processed.action_traces[0].console, `[\n  {\n    \"actor\": \"${name}\",\n    \"permission\": \"active\"\n  }\n]\n`);
    });

    it('publication_time', () => {
        var js_code = `exports.hi = v => {
            console.log(Date.now()* 1000 - action.publication_time);
        }`;
        set_code(name, fibos.compileCode(js_code));


        var r = hi('lion2', {
            authorization: name
        });

        assert.equal(r.processed.action_traces[0].console, '0\n');
    });

    it('id', () => {
        var js_code = `exports.hi = v => {
            console.log(action.id);
        }`;
        set_code(name, fibos.compileCode(js_code));


        var r = hi('lion', {
            authorization: name
        });

        assert.equal(r.processed.action_traces[0].console.trim(), r.transaction_id);
    });
});

require.main === module && test.run(console.DEBUG);