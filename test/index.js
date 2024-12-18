var test = require('test');
var test_util = require('./test_util');
let plugins_test_util = require("./plugins/test_util");

test.setup();

function test_db(name) {
    describe(name, () => {
        before(() => {
            test_util.name = name;
            test_util.node();
        });
        after(test_util.stop);

        run('./basic');
        run('./currency');
        run("./js_contract");
    });
}

function test_plugins(name) {
    describe(name, () => {
        before(() => {
            plugins_test_util.name = name;
            plugins_test_util.node();
        });
        after(plugins_test_util.stop);
        run('./plugins');
    });
}

test_db("fibos");
test_plugins("fibos");
test_db("eosio");

test.run(console.DEBUG);
