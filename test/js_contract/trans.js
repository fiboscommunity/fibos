var test = require('test');
test.setup();

var FIBOS = require('@tpblock/client')
var test_util = require('../test_util');
var encoding = require('encoding');

describe('trans', () => {
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

    function hi1(user, friend, auth) {
        return fibos.transact_sync({
            actions: [{
                account: name,
                name: 'hi1',
                authorization: [{
                    actor: auth.authorization,
                    permission: 'active',
                }],
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

        set_abi(name, abi);
        set_abi(name1, abi);

        var js_code = `exports.hi = user => {console.log(user);
            trans.send_inline("${name1}", "hi1", {user:"user1", friend:"user2"}, [{"actor": "${name}", "permission": "active"}]);};
        exports.hi1 = (user, friend) => {console.log(user, friend);trans.send_context_free_inline("${name1}", "hi1", {user:"user1", friend:"user2"});}`;
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
                    permission: "active",
                    parent: 'owner',
                    auth: {
                        threshold: 1,
                        accounts: [{
                            permission: {
                                actor: name,
                                permission: test_util.name + ".code"
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

        var js_code1 = `exports.hi1 = (user, friend) => console.log(action.has_auth("${name}"), action.has_auth("${name1}"), user, friend)`;
        set_code(name1, fibos.compileCode(js_code1));
    });
    after(() => {
        require.main === module && test_util.stop();
    });

    it('send_inline', () => {
        var r = hi('lion3213', {
            authorization: name
        });

        assert.equal(r.processed.action_traces[0].console, "lion3213\n");
        assert.equal(r.processed.action_traces[0].inline_traces[0].console, "true false user1 user2\n");
    });

    it('send_context_free_inline', () => {
        var r = hi1('lion13213', 'lion132131', {
            authorization: name
        });

        assert.equal(r.processed.action_traces[0].console, "lion13213 lion132131\n");
        assert.equal(r.processed.action_traces[0].inline_traces[0].console, "false false user1 user2\n");
    });

    it('send_inline without permission', () => {
        var js_code = `
            exports.hi = user => {
                console.log(user);
                trans.send_inline("${name1}", "hi1", {
                    user: "user1",
                    friend: "user2"
                });
            };`;

        set_code(name, fibos.compileCode(js_code));

        var r = hi('lion13213', {
            authorization: name
        });

        assert.equal(r.processed.action_traces[0].inline_traces[0].console, "false false user1 user2\n");
    });

    it('send_context_free_inline with permission', () => {
        var js_code = `
            exports.hi1 = (user, friend) => {
                console.log(user, friend);
                trans.send_context_free_inline("${name1}", "hi1", {
                    user: "user1",
                    friend: "user2"
                }, [{
                    actor: "${name}",
                    permission: "active"
                }]);
            }`;

        set_code(name, fibos.compileCode(js_code));

        assert.throws(() => {
            var r = hi1('lion3213', "lion132131", {
                authorization: name
            });
        });
    });

    it('call inline action not exposed in abi', () => {
        var js_code = `
            exports.hi = user => {
                console.log(user);
                trans.send_inline("${name1}", "hi2", {
                    user: "user1",
                    friend: "user2"
                }, [{
                    actor: "${name}",
                    permission: "active"
                }]);
            };`;

        set_code(name, fibos.compileCode(js_code));

        var js_code1 = `
            exports.hi2 = (user, friend) => {
                console.log(action.has_auth("${name}"), action.has_auth("${name1}"), user, friend)
            }`;
        set_code(name1, fibos.compileCode(js_code1));

        assert.throws(() => {
            hi('lion3213', {
                authorization: name
            });
        });
    });
});

require.main === module && test.run(console.DEBUG);