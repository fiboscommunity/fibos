let leftpad = require('leftpad');

exports.requirenpm = user => {
    assert.equal(leftpad(5, 10), "0000000005");
}

exports.requiremodule = user => {
    require('../module');
}