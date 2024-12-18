var test = require("test");
test.setup();

var FIBOS = require('@tpblock/client')
var test_util = require('../test_util');
var encoding = require('encoding');

describe('assert', () => {
    var ctx;
    var fibos;
    var name;
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

        set_abi(name, abi);
    });
    after(() => {
        require.main === module && test_util.stop();
    });

    it('isTrue', () => {
        var js_code = `
        exports.hi = user => {
            assert.isTrue(true);

            assert.throws(() => {
                assert.isTrue(false);
            }, "expected false to be true");

            assert.throws(() => {
                assert.isTrue(1);
            }, "expected 1 to be true");

            assert.throws(() => {
                assert.isTrue('test');
            }, "expected 'test' to be true");
        };`;
        set_code(name, fibos.compileCode(js_code));

        hi('lion1', {
            authorization: name
        });
    });

    it('ok', () => {
        var js_code = `
        exports.hi = user => {
            assert.ok(true);
            assert.ok(1);
            assert.ok('test');

            assert.throws(() => {
                assert.ok(false);
            }, "expected false to be truthy");

            assert.throws(() => {
                assert.ok(0);
            }, "expected 0 to be truthy");

            assert.throws(() => {
                assert.ok('');
            }, "expected '' to be truthy");
        };`;

        set_code(name, fibos.compileCode(js_code));

        hi('lion2', {
            authorization: name
        });
    });

    it('notOk', () => {
        var js_code = `
        exports.hi = user => {
            assert.notOk(false);
            assert.notOk(0);
            assert.notOk('');

            assert.throws(() => {
                assert.notOk(true);
            }, "expected true to be falsy");

            assert.throws(() => {
                assert.notOk(1);
            }, "expected 1 to be falsy");

            assert.throws(() => {
                assert.notOk('test');
            }, "expected 'test' to be falsy");
        };`;

        set_code(name, fibos.compileCode(js_code));

        hi('lion3', {
            authorization: name
        });
    });

    it('isFalse', () => {
        var js_code = `
        exports.hi = user => {
            assert.isFalse(false);

            assert.throws(() => {
                assert.isFalse(true);
            }, "expected true to be false");

            assert.throws(() => {
                assert.isFalse(0);
            }, "expected 0 to be false");
        };`;

        set_code(name, fibos.compileCode(js_code));

        hi('lion4', {
            authorization: name
        });
    });

    it('equal', () => {
        var js_code = `
        exports.hi = user => {
            var foo;
            assert.equal(foo, undefined);
        };`;

        set_code(name, fibos.compileCode(js_code));

        hi('lion5', {
            authorization: name
        });
    });

    it('isObject', () => {
        var js_code = `
        exports.hi = user => {
            function Foo() { }
            assert.isObject({});
            assert.isObject(new Foo());

            assert.throws(() => {
                assert.isObject(true);
            }, "expected true to be an object");

            assert.throws(() => {
                assert.isObject('foo');
            }, "expected 'foo' to be an object");
        };`;

        set_code(name, fibos.compileCode(js_code));

        hi('lion11', {
            authorization: name
        });
    });

    it('isNotObject', () => {
        var js_code = `
        exports.hi = user => {
            function Foo() { }
            assert.isNotObject(5);

            assert.throws(() => {
                assert.isNotObject({});
            }, "expected {} not to be an object");
        };`;

        set_code(name, fibos.compileCode(js_code));

        hi('lion12', {
            authorization: name
        });
    });

    it('notEqual', () => {
        var js_code = `
            exports.hi = user => {
                assert.notEqual(3, 4);

                assert.throws(() => {
                    assert.notEqual(5, 5);
                }, "expected 5 to not equal 5");
            };`;

        set_code(name, fibos.compileCode(js_code));

        hi('lion13', {
            authorization: name
        });
    });

    it('strictEqual', () => {
        var js_code = `
        exports.hi = user => {
            assert.strictEqual('foo', 'foo');

            assert.throws(() => {
                assert.strictEqual('5', 5);
            }, "expected \'5\' to equal 5");
        };`;

        set_code(name, fibos.compileCode(js_code));

        hi('lion14', {
            authorization: name
        });
    });

    it('notStrictEqual', () => {
        var js_code = `
        exports.hi = user => {
            assert.notStrictEqual(5, '5');

            assert.throws(() => {
                assert.notStrictEqual(5, 5);
            }, "expected 5 to not equal 5");
        };`;

        set_code(name, fibos.compileCode(js_code));

        hi('lion15', {
            authorization: name
        });
    });

    it('deepEqual', () => {
        var js_code = `
        exports.hi = user => {
            assert.deepEqual({
                tea: 'chai'
            }, {
                    tea: 'chai'
                });
    
            assert.throws(() => {
                assert.deepEqual({
                    tea: 'chai'
                }, {
                        tea: 'black'
                    });
            },
                "expected { tea: \'chai\' } to deeply equal { tea: \'black\' }");
    
            var obja = Object.create({
                tea: 'chai'
            }),
                objb = Object.create({
                    tea: 'chai'
                });
    
            assert.deepEqual(obja, objb);
    
            var obj1 = Object.create({
                tea: 'chai'
            }),
                obj2 = Object.create({
                    tea: 'black'
                });
    
            assert.throws(() => {
                assert.deepEqual(obj1, obj2);
            },
                "expected { tea: \'chai\' } to deeply equal { tea: \'black\' }");
    
            assert.throws(() => {
                assert.deepEqual({
                    "100": 2,
                    "5": 2
                }, {
                        "100": 2,
                        "5": 4
                    });
            });
    
            assert.throws(() => {
                assert.deepEqual({
                    "100": 2,
                    "5": 2
                }, {
                        "1": 2,
                        "5": 2
                    });
            });
    
            assert.notDeepEqual({
                "100": 2,
                "5": 2
            }, {
                    "1": 2,
                    "5": 4
                });
    
            assert.throws(() => {
                assert.notDeepEqual({
                    "100": 2,
                    "5": 2
                }, {
                        "100": 2,
                        "5": 2
                    });
            });
        };`;

        set_code(name, fibos.compileCode(js_code));

        hi('lion21', {
            authorization: name
        });
    });

    it('deepEqual (ordering)', () => {
        var js_code = `
        exports.hi = user => {
            var a = {
                a: 'b',
                c: 'd'
            },
                b = {
                    c: 'd',
                    a: 'b'
                };
            assert.deepEqual(a, b);
        };`;

        set_code(name, fibos.compileCode(js_code));

        hi('lion22', {
            authorization: name
        });
    });

    it('deepEqual /regexp/', () => {
        var js_code = `
        exports.hi = user => {
            assert.deepEqual(/a/, /a/);
            assert.notDeepEqual(/a/, /b/);
            assert.notDeepEqual(/a/, {});
            assert.deepEqual(/a/g, /a/g);
            assert.notDeepEqual(/a/g, /b/g);
            assert.deepEqual(/a/i, /a/i);
            assert.notDeepEqual(/a/i, /b/i);
            assert.deepEqual(/a/m, /a/m);
            assert.notDeepEqual(/a/m, /b/m);
        };`;

        set_code(name, fibos.compileCode(js_code));

        hi('lion23', {
            authorization: name
        });
    });

    it('deepEqual (Date)', () => {
        var js_code = `
        exports.hi = user => {
            var a = new Date(1, 2, 3),
            b = new Date(4, 5, 6);
            assert.deepEqual(a, a);
            assert.notDeepEqual(a, b);
            assert.notDeepEqual(a, {});
        };`;

        set_code(name, fibos.compileCode(js_code));

        hi('lion24', {
            authorization: name
        });
    });

    it('deepEqual (Buffer)', () => {
        var js_code = `
        exports.hi = user => {
            var a = new Buffer("123"),
            b = new Buffer("456");
            assert.deepEqual(a, a);
            assert.notDeepEqual(a, b);
            assert.notDeepEqual(a, {});
        };`;

        set_code(name, fibos.compileCode(js_code));

        hi('lion25', {
            authorization: name
        });
    });

    it('deepEqual (circular)', () => {
        var js_code = `
        exports.hi = user => {
            var circularObject = {},
            secondCircularObject = {};
            circularObject.field = circularObject;
            secondCircularObject.field = secondCircularObject;

            assert.deepEqual(circularObject, secondCircularObject);

            assert.throws(() => {
                secondCircularObject.field2 = secondCircularObject;
                assert.deepEqual(circularObject,
                    secondCircularObject);
            }, "expected { field: [Circular] } to deeply equal { Object (field, field2) }");
        };`;

        set_code(name, fibos.compileCode(js_code));

        hi('lion31', {
            authorization: name
        });
    });

    it('notDeepEqual', () => {
        var js_code = `
        exports.hi = user => {
            assert.notDeepEqual({
                tea: 'jasmine'
            }, {
                    tea: 'chai'
                });
    
            assert.throws(() => {
                assert.notDeepEqual({
                    tea: 'chai'
                }, {
                        tea: 'chai'
                    });
            }, "expected { tea: \'chai\' } to not deeply equal { tea: \'chai\' }");
        };`;

        set_code(name, fibos.compileCode(js_code));

        hi('lion32', {
            authorization: name
        });
    });

    it('notDeepEqual (circular)', () => {
        var js_code = `
        exports.hi = user => {
            var circularObject = {},
            secondCircularObject = {
                tea: 'jasmine'
            };
            circularObject.field = circularObject;
            secondCircularObject.field = secondCircularObject;

            assert.notDeepEqual(circularObject,
                secondCircularObject);

            assert.throws(() => {
                delete secondCircularObject.tea;
                assert.notDeepEqual(circularObject,
                    secondCircularObject);
            }, "expected { field: [Circular] } to not deeply equal { field: [Circular] }");
        };`;

        set_code(name, fibos.compileCode(js_code));

        hi('lion33', {
            authorization: name
        });
    });

    it('isNull', () => {
        var js_code = `
        exports.hi = user => {
            assert.isNull(null);

            assert.throws(() => {
                assert.isNull(undefined);
            }, "expected undefined to equal null");
        };`;

        set_code(name, fibos.compileCode(js_code));

        hi('lion34', {
            authorization: name
        });
    });

    it('isNotNull', () => {
        var js_code = `
        exports.hi = user => {
            assert.isNotNull(undefined);

            assert.throws(() => {
                assert.isNotNull(null);
            }, "expected null to not equal null");
        };`;

        set_code(name, fibos.compileCode(js_code));

        hi('lion35', {
            authorization: name
        });
    });

    it('isUndefined', () => {
        var js_code = `
        exports.hi = user => {
            assert.isUndefined(undefined);

            assert.throws(() => {
                assert.isUndefined(null);
            }, "expected null to equal undefined");
        };`;

        set_code(name, fibos.compileCode(js_code));

        hi('lion41', {
            authorization: name
        });
    });

    it('isDefined', () => {
        var js_code = `
        exports.hi = user => {
            assert.isDefined(null);

            assert.throws(() => {
                assert.isDefined(undefined);
            }, "expected undefined to not equal undefined");
        };`;

        set_code(name, fibos.compileCode(js_code));

        hi('lion42', {
            authorization: name
        });
    });

    it('isFunction', () => {
        var js_code = `
        exports.hi = user => {
            var func = () => { };
            assert.isFunction(func);

            assert.throws(() => {
                assert.isFunction({});
            }, "expected {} to be a function");
        };`;

        set_code(name, fibos.compileCode(js_code));

        hi('lion43', {
            authorization: name
        });
    });

    it('isNotFunction', () => {
        var js_code = `
        exports.hi = user => {
            assert.isNotFunction(5);

            assert.throws(() => {
                assert.isNotFunction(() => { });
            }, "expected [Function] not to be a function");
        };`;

        set_code(name, fibos.compileCode(js_code));

        hi('lion44', {
            authorization: name
        });
    });

    it('isArray', () => {
        var js_code = `
        exports.hi = user => {
            assert.isArray([]);
            assert.isArray(new Array);

            assert.throws(() => {
                assert.isArray({});
            }, "expected {} to be an array");
        };`;

        set_code(name, fibos.compileCode(js_code));

        hi('lion45', {
            authorization: name
        });
    });

    it('isNotArray', () => {
        var js_code = `
        exports.hi = user => {
            assert.isNotArray(3);

            assert.throws(() => {
                assert.isNotArray([]);
            }, "expected [] not to be an array");

            assert.throws(() => {
                assert.isNotArray(new Array);
            }, "expected [] not to be an array");
        };`;

        set_code(name, fibos.compileCode(js_code));

        hi('lion51', {
            authorization: name
        });
    });

    it('isString', () => {
        var js_code = `
        exports.hi = user => {
            assert.isString('Foo');
            assert.isString(new String('foo'));

            assert.throws(() => {
                assert.isString(1);
            }, "expected 1 to be a string");
        };`;

        set_code(name, fibos.compileCode(js_code));

        hi('lion52', {
            authorization: name
        });
    });

    it('isNotString', () => {
        var js_code = `
        exports.hi = user => {
            assert.isNotString(3);
            assert.isNotString(['hello']);

            assert.throws(() => {
                assert.isNotString('hello');
            }, "expected 'hello' not to be a string");
        };`;

        set_code(name, fibos.compileCode(js_code));

        hi('lion53', {
            authorization: name
        });
    });

    it('isNumber', () => {
        var js_code = `
        exports.hi = user => {
            assert.isNumber(1);
            assert.isNumber(Number('3'));

            assert.throws(() => {
                assert.isNumber('1');
            }, "expected \'1\' to be a number");
        };`;

        set_code(name, fibos.compileCode(js_code));

        hi('lion54', {
            authorization: name
        });
    });

    it('isNotNumber', () => {
        var js_code = `
        exports.hi = user => {
            assert.isNotNumber('hello');
            assert.isNotNumber([5]);

            assert.throws(() => {
                assert.isNotNumber(4);
            }, "expected 4 not to be a number");
        };`;

        set_code(name, fibos.compileCode(js_code));

        hi('lion55', {
            authorization: name
        });
    });

    it('isBoolean', () => {
        var js_code = `
        exports.hi = user => {
            assert.isBoolean(true);
            assert.isBoolean(false);
    
            assert.throws(() => {
                assert.isBoolean('1');
            }, "expected \'1\' to be a boolean");
        };`;

        set_code(name, fibos.compileCode(js_code));

        hi('lion551', {
            authorization: name
        });
    });

    it('isNotBoolean', () => {
        var js_code = `
        exports.hi = user => {
            assert.isNotBoolean('true');

            assert.throws(() => {
                assert.isNotBoolean(true);
            }, "expected true not to be a boolean");

            assert.throws(() => {
                assert.isNotBoolean(false);
            }, "expected false not to be a boolean");
        };`;

        set_code(name, fibos.compileCode(js_code));

        hi('lion552', {
            authorization: name
        });
    });

    it('property', () => {
        var js_code = `
        exports.hi = user => {
            var obj = {
                foo: {
                    bar: 'baz'
                }
            };
            var simpleObj = {
                foo: 'bar'
            };
            assert.property(obj, 'foo');
            assert.deepProperty(obj, 'foo.bar');
            assert.notProperty(obj, 'baz');
            assert.notProperty(obj, 'foo.bar');
            assert.notDeepProperty(obj, 'foo.baz');
            assert.deepPropertyVal(obj, 'foo.bar', 'baz');
            assert.deepPropertyNotVal(obj, 'foo.bar', 'flow');
    
            assert.throws(() => {
                assert.property(obj, 'baz');
            }, "expected { foo: { bar: 'baz' } } to have a property 'baz'");
    
            assert.throws(() => {
                assert.deepProperty(obj, 'foo.baz');
            }, "expected { foo: { bar: 'baz' } } to have a deep property 'foo.baz'");
    
            assert.throws(() => {
                assert.notProperty(obj, 'foo');
            }, "expected { foo: { bar: 'baz' } } to not have property 'foo'");
    
            assert.throws(() => {
                assert.notDeepProperty(obj, 'foo.bar');
            }, "expected { foo: { bar: 'baz' } } to not have deep property 'foo.bar'");
    
            assert.throws(() => {
                assert.propertyVal(simpleObj, 'foo', 'ball');
            }, "expected { foo: 'bar' } to have a property 'foo' of 'ball', but got 'bar'");
    
            assert.throws(() => {
                assert.deepPropertyVal(obj, 'foo.bar', 'ball');
            }, "expected { foo: { bar: 'baz' } } to have a deep property 'foo.bar' of 'ball', but got 'baz'");
    
            assert.throws(() => {
                assert.propertyNotVal(simpleObj, 'foo', 'bar');
            }, "expected { foo: 'bar' } to not have a property 'foo' of 'bar'");
    
            assert.throws(() => {
                assert.deepPropertyNotVal(obj, 'foo.bar', 'baz');
            }, "expected { foo: { bar: 'baz' } } to not have a deep property 'foo.bar' of 'baz'");
        };`;

        set_code(name, fibos.compileCode(js_code));

        hi('lion553', {
            authorization: name
        });
    });

    it('throws', () => {
        var js_code = `
        exports.hi = user => {
            assert.throws(() => {
                throw new Error('foo');
            });
            assert.throws(() => {
                throw new Error('bar');
            }, 'bar');
            assert.throws(() => {
                throw new Error('bar');
            }, /bar/);
    
            assert.throws(() => {
                assert.throws(() => { });
            }, "expected [Function] to throw an error");
        };`;

        set_code(name, fibos.compileCode(js_code));

        hi('lion554', {
            authorization: name
        });
    });

    it('doesNotThrow', () => {
        var js_code = `
        exports.hi = user => {
            assert.doesNotThrow(() => { });
            assert.doesNotThrow(() => { }, 'foo');

            assert.throws(() => {
                assert.doesNotThrow(() => {
                    throw new Error('foo');
                });
            }, 'expected [Function] to not throw an error but [Error: foo] was thrown');
        };`;

        set_code(name, fibos.compileCode(js_code));

        hi('lion555', {
            authorization: name
        });
    });

    it('closeTo', () => {
        var js_code = `
        exports.hi = user => {
            assert.closeTo(1.5, 1.0, 0.5);
            assert.closeTo(10, 20, 20);
            assert.closeTo(-10, 20, 30);

            assert.throws(() => {
                assert.closeTo(2, 1.0, 0.5);
            }, "expected 2 to be close to 1 +/- 0.5");

            assert.throws(() => {
                assert.closeTo(-10, 20, 29);
            }, "expected -10 to be close to 20 +/- 29");
        };`;

        set_code(name, fibos.compileCode(js_code));

        hi('lion5551', {
            authorization: name
        });
    });

    it('ifError', () => {
        var js_code = `
        exports.hi = user => {
            assert.throws(() => assert.ifError(1));
            assert.throws(() => assert.ifError('a'));
            assert.throws(() => assert.ifError(new Error('error')));
            assert.throws(function () { assert.ifError(new Error('test error')); });
            assert.doesNotThrow(function () { assert.ifError(null); });
            assert.doesNotThrow(function () { assert.ifError(); });

            let threw = false;
            try {
                assert.throws(() => assert.ifError(null));
            } catch (e) {
                threw = true;
                assert.strictEqual(e.message, 'Missing expected exception.');
            }
            assert.ok(threw);
        };`;

        set_code(name, fibos.compileCode(js_code));

        hi('lion5552', {
            authorization: name
        });
    });
});

require.main === module && test.run(console.DEBUG);