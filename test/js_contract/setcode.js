var test = require('test');
test.setup();

var FIBOS = require('@tpblock/client')
var test_util = require('../test_util');
var encoding = require('encoding');

describe('setcode', () => {
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

    before(() => {
        require.main === module && test_util.node();
        fibos = FIBOS(test_util.config);
        name = test_util.user(fibos);
    });
    after(() => {
        require.main === module && test_util.stop();
    });

    it('not zip data', () => {
        assert.throws(() => {
            fibos.setcodeSync(name, 0, 0, new Buffer('wrong data'));
        });
    });

    xit('not js code', () => {
        assert.throws(() => {
            set_code(fibos.compileCode('wrong script'));
        });
    });

    it('right js code', () => {
        var js_code = `exports.hi = user => console.log(user);`;
        set_code(fibos.compileCode(js_code));

        assert.throws(() => {
            set_code(fibos.compileCode(js_code));
            set_code(fibos.compileCode(js_code));
        });
    });

    var last_code;
    it('update js code', () => {
        var js_code = `exports.hi = user => console.error('in contract:', user);`;
        last_code = fibos.compileCode(js_code);
        set_code(last_code);
    });

    it('get code', () => {
        var js_code = `exports.hi = user => console.error('in contract:', user);`;
        var code = fibos.rpc.get_raw_code_and_abi_sync(name);
        assert.deepEqual(Buffer.from(code.wasm, 'base64'), last_code);
    });

    it('setabi', () => {
        set_abi(abi);
    });
});

require.main === module && test.run(console.DEBUG);