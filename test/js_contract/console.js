var test = require('test');
test.setup();

var FIBOS = require('@tpblock/client')
var test_util = require('../test_util');
var encoding = require('encoding');

describe('console', () => {
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

    it('log with fmt', () => {
        var js_code = `exports.hi = v => { console.log('hello :%s',"FIBOS") };`;
        set_code(name, fibos.compileCode(js_code));
        var r = hi('ljon1', {
            authorization: name1
        });
        assert.equal(r.processed.action_traces[0].console, 'hello :FIBOS\n');
    });

    it('log without fmt', () => {
        var js_code = `exports.hi = v => { console.log('hello FIBOS') };`;
        set_code(name, fibos.compileCode(js_code));

        var r = hi('liou12', {
            authorization: name1
        });
        assert.equal(r.processed.action_traces[0].console, 'hello FIBOS\n');
    });

    it('debug with fmt', () => {
        var js_code = `exports.hi = v => { console.debug('debug %s','FIBOS') };`;
        set_code(name, fibos.compileCode(js_code));

        var r = hi('lioj123', {
            authorization: name1
        });
        assert.equal(r.processed.action_traces[0].console, 'debug FIBOS\n');
    });

    it('debug without fmt', () => {
        var js_code = `exports.hi = v => { console.debug('debug FIBOS') };`;
        set_code(name, fibos.compileCode(js_code));

        var r = hi('uion1234', {
            authorization: name1
        });
        assert.equal(r.processed.action_traces[0].console, 'debug FIBOS\n');
    });

    it('info with fmt', () => {
        var js_code = `exports.hi = v => { console.info('info :%s','FIBOS') };`;
        set_code(name, fibos.compileCode(js_code));

        var r = hi('liom12', {
            authorization: name1
        });
        assert.equal(r.processed.action_traces[0].console, 'info :FIBOS\n');
    });

    it('info without fmt', () => {
        var js_code = `exports.hi = v => { console.info('info FIBOS') };`;
        set_code(name, fibos.compileCode(js_code));

        var r = hi('lign1123', {
            authorization: name1
        });
        assert.equal(r.processed.action_traces[0].console, 'info FIBOS\n');
    });

    it('notice with fmt', () => {
        var js_code = `exports.hi = v => { console.notice('notice :%s','FIBOS') };`;
        set_code(name, fibos.compileCode(js_code));

        var r = hi('leon1', {
            authorization: name1
        });
        assert.equal(r.processed.action_traces[0].console, 'notice :FIBOS\n');
    });

    it('notice without fmt', () => {
        var js_code = `exports.hi = v => { console.notice('notice FIBOS') };`;
        set_code(name, fibos.compileCode(js_code));

        var r = hi('mion1234', {
            authorization: name1
        });
        assert.equal(r.processed.action_traces[0].console, 'notice FIBOS\n');
    });

    it('warn with fmt', () => {
        var js_code = `exports.hi = v => { console.warn('warn :%s','FIBOS') };`;
        set_code(name, fibos.compileCode(js_code));

        var r = hi('lifn14', {
            authorization: name1
        });
        assert.equal(r.processed.action_traces[0].console, 'warn :FIBOS\n');
    });

    it('warn without fmt', () => {
        var js_code = `exports.hi = v => { console.warn('warn FIBOS') };`;
        set_code(name, fibos.compileCode(js_code));

        var r = hi('libn123', {
            authorization: name1
        });
        assert.equal(r.processed.action_traces[0].console, 'warn FIBOS\n');
    });

    it('error with fmt', () => {
        var js_code = `exports.hi = v => { console.error('error :%s','FIBOS') };`;
        set_code(name, fibos.compileCode(js_code));

        var r = hi('laon12', {
            authorization: name1
        });
        assert.equal(r.processed.action_traces[0].console, 'error :FIBOS\n');
    });

    it('error without fmt', () => {
        var js_code = `exports.hi = v => { console.error('error FIBOS') };`;
        set_code(name, fibos.compileCode(js_code));

        var r = hi('lioc1222', {
            authorization: name1
        });
        assert.equal(r.processed.action_traces[0].console, 'error FIBOS\n');
    });

    it('crit with fmt', () => {
        var js_code = `exports.hi = v => { console.crit('crit :%s','FIBOS') };`;
        set_code(name, fibos.compileCode(js_code));

        var r = hi('liqn1232', {
            authorization: name1
        });
        assert.equal(r.processed.action_traces[0].console, 'crit :FIBOS\n');
    });

    it('crit without fmt', () => {
        var js_code = `exports.hi = v => { console.crit('crit FIBOS') };`;
        set_code(name, fibos.compileCode(js_code));

        var r = hi('lgon1', {
            authorization: name1
        });
        assert.equal(r.processed.action_traces[0].console, 'crit FIBOS\n');
    });

    it('alert with fmt', () => {
        var js_code = `exports.hi = v => { console.alert('alert :%s','FIBOS') };`;
        set_code(name, fibos.compileCode(js_code));

        var r = hi('lipn12', {
            authorization: name1
        });
        assert.equal(r.processed.action_traces[0].console, 'alert :FIBOS\n');
    });

    it('alert without fmt', () => {
        var js_code = `exports.hi = v => { console.alert('alert FIBOS') };`;
        set_code(name, fibos.compileCode(js_code));

        var r = hi('lioi123', {
            authorization: name1
        });
        assert.equal(r.processed.action_traces[0].console, 'alert FIBOS\n');
    });

    it('dir', () => {
        var js_code = `exports.hi = v => { var a = {}; console.dir(a); };`;
        set_code(name, fibos.compileCode(js_code));

        var r = hi('lior13', {
            authorization: name1
        });
        assert.equal(r.processed.action_traces[0].console, '{}\n');
    });

    it('trace', () => {
        var js_code = `exports.hi = v => { var a = {}; console.trace(a); };`;
        set_code(name, fibos.compileCode(js_code));

        var r = hi('liln1345', {
            authorization: name1
        });
        assert.exist(r.processed.action_traces[0].console, 'console.trace: [object Object]\n');
    });
});

require.main === module && test.run(console.DEBUG);