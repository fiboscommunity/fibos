var test = require('test');
test.setup();

var FIBOS = require('@tpblock/client')
var test_util = require('./test_util');

describe('Require', () => {
    var name;
    var name1;
    var fibos;
    var abi = {
        "version": "eosio::abi/1.0",
        "structs": [{
            "name": "requirenpm",
            "base": "",
            "fields": [{
                "name": "user",
                "type": "name"
            }]
        }],
        "actions": [{
            "name": "requirenpm",
            "type": "requirenpm",
            "ricardian_contract": ""
        }]
    };

    before(() => {
        require.main === module && test_util.node();
        fibos = FIBOS(test_util.config);
        name = test_util.user(fibos);
    });
    after(() => {
        require.main === module && test_util.stop();
    });

    it('reuqire npm module', () => {
        fibos.setabiSync(name, abi);
        fibos.setcodeSync(name, 0, 0, fibos.compileModule(__dirname + "/modulewithnpm"));

        var ctx = fibos.contractSync(name);
        ctx.requirenpmSync('lion', {
            authorization: name
        });
    });

    it('reuqire module not compiled should throw', () => {
        var ctx = fibos.contractSync(name);
        assert.throws(() => {
            ctx.requiremoduleSync('lion', {
                authorization: name
            });
        });
    });
});

require.main === module && test.run(console.DEBUG);