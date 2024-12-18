const http = require('http');

var test = require('test');
test.setup();

var test_util = require("./test_util");
describe('chain.post', () => {

    before(() => {
        require.main === module && test_util.node();
    });

    after(() => {
        require.main === module && test_util.stop();
    });

    it('get_info', () => {
        var http_plugin_result = http.get(test_util.config.httpEndpoint + '/v1/chain/get_info');
        var chain_svr_result = http.post(test_util.config.chainSvrEndpoint, {
            json: {
                url: '/v1/chain/get_info'
            }
        });

        assert.deepEqual(http_plugin_result.json(), chain_svr_result.json());
    });

});

require.main === module && test.run(console.DEBUG);
