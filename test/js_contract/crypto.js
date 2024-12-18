var test = require('test');
test.setup();

var FIBOS = require('@tpblock/client')
var test_util = require('../test_util');
var encoding = require('encoding');

describe('sandbox', () => {
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
        }, {
            "name": "hi1",
            "base": "",
            "fields": [{
                "name": "sig",
                "type": "signature"
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

    function hi1(sig, auth) {
        return fibos.transact_sync({
            actions: [{
                account: name,
                name: 'hi1',
                authorization: [{
                    actor: auth.authorization,
                    permission: 'active',
                }],
                "data": {
                    "sig": sig
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
    });
    after(() => {
        require.main === module && test_util.stop();
    });

    it('sha1', () => {
        var js_code = `exports.hi = v => {
            console.error(crypto.sha1("abcdefg"));
        }`;
        set_code(name, fibos.compileCode(js_code));

        var r = hi('sha1', {
            authorization: name
        });
        assert.equal(r.processed.action_traces[0].console, '2fb5e13419fc89246865e7a324f476ec624e8740\n');
    });

    it('sha256', () => {
        var js_code = `exports.hi = v => {
            console.error(crypto.sha256("abcdefg"));
        }`;
        set_code(name, fibos.compileCode(js_code));

        var r = hi('sha2', {
            authorization: name
        });
        assert.equal(r.processed.action_traces[0].console, '7d1a54127b222502f5b79b5fb0803061152a44f92b37e23c6527baf665d4da9a\n');
    });

    it('sha512', () => {
        var js_code = `exports.hi = v => {
            console.error(crypto.sha512("abcdefg"));
        }`;
        set_code(name, fibos.compileCode(js_code));

        var r = hi('sha3', {
            authorization: name
        });
        assert.equal(r.processed.action_traces[0].console, 'd716a4188569b68ab1b6dfac178e570114cdf0ea3a1cc0e31486c3e41241bc6a76424e8c37ab26f096fc85ef9886c8cb634187f4fddff645fb099f1ff54c6b8c\n');
    });

    it('ripemd160', () => {
        var js_code = `exports.hi = v => {
            console.error(crypto.ripemd160("abcdefg"));
        }`;
        set_code(name, fibos.compileCode(js_code));

        var r = hi('ripemd', {
            authorization: name
        });
        assert.equal(r.processed.action_traces[0].console, '874f9960c5d2b7a9b5fad383e1ba44719ebb743a\n');
    });

    it('recover_key', () => {
        var ecc = FIBOS.ecc;
        var pk = ecc.randomKey_sync(undefined, { secureEnv: true });
        var s = ecc.sign('I am alive', pk);

        var js_code = `exports.hi1 = sig => {
            console.log(crypto.recover_key(crypto.sha256('I am alive'), sig));
        }`;
        set_code(name, fibos.compileCode(js_code));

        var r = hi1(s, {
            authorization: name
        });
        assert.equal(r.processed.action_traces[0].console, ecc.privateToPublic(pk) + '\n');
    });
});

require.main === module && test.run(console.DEBUG);

