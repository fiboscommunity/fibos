var test = require('test');
test.setup();

var FIBOS = require('@tpblock/client')
var test_util = require('../test_util');
var encoding = require('encoding');

describe('sandbox', () => {
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
        "tables": [{
            "name": "players",
            "type": "player",
            "index_type": "i64",
            "key_names": ["nickname"],
            "key_types": ["int64"]
        }],
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
    });
    after(() => {
        require.main === module && test_util.stop();
    });

    it('share variable in module', () => {
        var js_code = `var test = 100;exports.hi = v => {
            test ++;
            console.error(test);
        }`;
        set_code(name, fibos.compileCode(js_code));

        var r = hi('test1', {
            authorization: name
        });
        assert.equal(r.processed.action_traces[0].console, '101\n');

        var r = hi('test2', {
            authorization: name
        });
        assert.equal(r.processed.action_traces[0].console, '101\n');
    });

    it('global object', () => {
        var js_code = `exports.hi = v => {
            String.test = 100;
            console.error(String.test);
        }`;
        set_code(name, fibos.compileCode(js_code));

        var r = hi('lion', {
            authorization: name
        });
        assert.equal(r.processed.action_traces[0].console, 'undefined\n');
    });

    it('native object', () => {
        var js_code = `exports.hi = v => {
            action.test = 100;
            console.error(action.test);
        }`;
        set_code(name, fibos.compileCode(js_code));

        var r = hi('test3', {
            authorization: name
        });
        assert.equal(r.processed.action_traces[0].console, 'undefined\n');
    });

    it('bc console module', () => {
        var js_code = `exports.hi = v => {
            console.readLine();
        }`;
        set_code(name, fibos.compileCode(js_code));

        assert.throws(() => {
            var r = hi('test4', {
                authorization: name
            });
        });
    });

    it('math module', () => {
        var js_code = `exports.hi = v => {
            console.log(Math.random);
        }`;
        set_code(name, fibos.compileCode(js_code));

        var r = hi('test5', {
            authorization: name
        });

        assert.equal(r.processed.action_traces[0].console, 'undefined\n');
    });

    it('WebAssembly module', () => {
        var js_code = `exports.hi = v => {
            console.log(typeof WebAssembly);
        }`;
        set_code(name, fibos.compileCode(js_code));

        var r = hi('wasm', {
            authorization: name
        });

        assert.equal(r.processed.action_traces[0].console, 'undefined\n');
    });

    it('Date object', () => {
        var js_code = `exports.hi = v => {
            console.log(Date.now()* 1000 - action.publication_time);
        }`;
        set_code(name, fibos.compileCode(js_code));

        var r = hi('test11', {
            authorization: name
        });

        assert.equal(r.processed.action_traces[0].console, '0\n');
    });

    it('timezone object', () => {
        var js_code = `exports.hi = v => {
            console.log(new Date().getTimezoneOffset());
        }`;
        set_code(name, fibos.compileCode(js_code));

        var r = hi('test111', {
            authorization: name
        });

        assert.equal(r.processed.action_traces[0].console, '0\n');
    });

    it('date part', () => {
        var js_code = `exports.hi = v => {
            var date = new Date(0);
            console.log({
                FullYear: date.getFullYear(),
                Month: date.getMonth(),
                Date: date.getDate(),
                Hours: date.getHours(),
                Minutes: date.getMinutes(),
                Seconds: date.getSeconds(),
                Milliseconds: date.getMilliseconds()
            });
        }`;
        set_code(name, fibos.compileCode(js_code));

        var r = hi('test1234', {
            authorization: name
        });

        assert.equal(r.processed.action_traces[0].console,
            '{\n  \"FullYear\": 1970,\n  \"Month\": 0,\n  \"Date\": 1,\n  \"Hours\": 0,\n  \"Minutes\": 0,\n  \"Seconds\": 0,\n  \"Milliseconds\": 0\n}\n');
    })

    it('Intl', () => {
        var js_code = `exports.hi = v => {
            console.log(typeof Intl);
        }`;
        set_code(name, fibos.compileCode(js_code));

        var r = hi('test2355', {
            authorization: name
        });

        assert.equal(r.processed.action_traces[0].console, 'undefined\n');
    });

    it('toLocaleString', () => {
        var js_code = `exports.hi = v => {
            console.log(Object.prototype.toLocaleString);
            console.log(Number.prototype.toLocaleString);
            console.log(Date.prototype.toLocaleString);
            console.log(Date.prototype.toLocaleDateString);
            console.log(Date.prototype.toLocaleTimeString);
            console.log(BigInt.prototype.toLocaleString);
            console.log(Array.prototype.toLocaleString);
            console.log(Int8Array.prototype.toLocaleString);
            console.log(Uint8Array.prototype.toLocaleString);
            console.log(Uint8ClampedArray.prototype.toLocaleString);
            console.log(Int16Array.prototype.toLocaleString);
            console.log(Uint16Array.prototype.toLocaleString);
            console.log(Int32Array.prototype.toLocaleString);
            console.log(Uint32Array.prototype.toLocaleString);
            console.log(Float32Array.prototype.toLocaleString);
            console.log(Float64Array.prototype.toLocaleString);
            console.log(BigInt64Array.prototype.toLocaleString);
            console.log(BigUint64Array.prototype.toLocaleString);
        }`;
        set_code(name, fibos.compileCode(js_code));

        var r = hi('test1245', {
            authorization: name
        });

        assert.equal(r.processed.action_traces[0].console,
            'undefined\nundefined\nundefined\nundefined\nundefined\nundefined\nundefined\nundefined\nundefined\nundefined\nundefined\nundefined\nundefined\nundefined\nundefined\nundefined\nundefined\nundefined\n');
    });

    it('localeCompare', () => {
        var js_code = `exports.hi = v => {
            console.log(String.prototype.localeCompare);
        }`;
        set_code(name, fibos.compileCode(js_code));

        var r = hi('test225', {
            authorization: name
        });

        assert.equal(r.processed.action_traces[0].console, 'undefined\n');
    });

    it('localeCase', () => {
        var js_code = `exports.hi = v => {
            console.log(String.prototype.toLocaleLowerCase);
            console.log(String.prototype.toLocaleUpperCase);
        }`;
        set_code(name, fibos.compileCode(js_code));

        var r = hi('test2251', {
            authorization: name
        });

        assert.equal(r.processed.action_traces[0].console, 'undefined\nundefined\n');
    });

    it('local toString', () => {
        var js_code = `exports.hi = v => {
            console.log(new Date(0).toString());
        }`;
        set_code(name, fibos.compileCode(js_code));

        var r = hi('test2252', {
            authorization: name
        });

        assert.equal(r.processed.action_traces[0].console, 'Thu Jan 01 1970 00:00:00 GMT+0000 (UTC)\n');
    });

    it('big data', () => {
        var js_code = `exports.hi = v => {
            var data = [];
            while(new Uint8Array(1000000000));
        }`;
        set_code(name, fibos.compileCode(js_code));

        assert.throws(() => {
            var r = hi('test12', {
                authorization: name
            });
        });
    });
});

require.main === module && test.run(console.DEBUG);