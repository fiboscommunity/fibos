var test = require('test');
test.setup();

var FIBOS = require('@tpblock/client')
var test_util = require('../test_util');
var encoding = require('encoding');

describe('Buffer', () => {
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
    });
    after(() => {
        require.main === module && test_util.stop();
    });

    it("new Buffer(Integer)", () => {
        var js_code = `exports.hi = v => {
            var buf = new Buffer(100);
            console.log(buf.length);
        }`;
        set_code(name, fibos.compileCode(js_code));

        var r = hi('buf', {
            authorization: name
        });
        assert.equal(r.processed.action_traces[0].console, '100\n');
    });

    it("new Buffer(Array)", () => {
        var js_code = `exports.hi = v => {
            var buf = new Buffer([0x31, 0x32, 0x33, 0x34])
            console.log(buf.toString());
        }`;
        set_code(name, fibos.compileCode(js_code));

        var r = hi('bufa', {
            authorization: name
        });
        assert.equal(r.processed.action_traces[0].console, '1234\n');
    });

    it("new Buffer(Array) with undefined encoding", () => {
        var js_code = `exports.hi = v => {
            var buf = new Buffer([0x31, 0x32, 0x33, 0x34], undefined);
            console.log(buf.toString());
        }`;
        set_code(name, fibos.compileCode(js_code));

        var r = hi('bufapol', {
            authorization: name
        });
        assert.equal(r.processed.action_traces[0].console, '1234\n');
    });

    it("new Buffer(Array) with encoding", () => {
        var js_code = `exports.hi = v => {
            var buf = new Buffer("厉害！","utf8");
            console.log(buf.toString());
        }`;
        set_code(name, fibos.compileCode(js_code));

        var r = hi('bohgfpd', {
            authorization: name
        });
        assert.equal(r.processed.action_traces[0].console, '厉害！\n');
    });

    it("new Buffer(TypedArray)", () => {
        var js_code = `exports.hi = v => {
            var arr = new Uint8Array(2);
            arr[0] = 50;
            arr[1] = 40;
            var buf = new Buffer(arr);
            console.log(buf.hex());

            var arr = new Uint8Array([0x10, 0x20, 0x30]);
            var arr1 = new Uint8Array(arr.buffer, 1, 2);
            var buf = new Buffer(arr1);
            console.log(buf.hex());
        }`;
        set_code(name, fibos.compileCode(js_code));

        var r = hi('bufc', {
            authorization: name
        });
        assert.equal(r.processed.action_traces[0].console, '3228\n2030\n');
    });

    it("new Buffer(ArrayBuffer)", () => {
        var js_code = `exports.hi = v => {
            var arr = new Uint16Array(2);
            arr[0] = 5000;
            arr[1] = 4000;
            var buf = new Buffer(arr.buffer);
            console.log(buf.hex());
        }`;
        set_code(name, fibos.compileCode(js_code));

        var r = hi('budd', {
            authorization: name
        });
        assert.equal(r.processed.action_traces[0].console, '8813a00f\n');
    });

    it("new Buffer(Buffer)", () => {
        var js_code = `exports.hi = v => {
            var buf = new Buffer(new Buffer("abcd"));
            console.log(buf.toString());
        }`;
        set_code(name, fibos.compileCode(js_code));

        var r = hi('buff', {
            authorization: name
        });
        assert.equal(r.processed.action_traces[0].console, 'abcd\n');
    });

    it("isBuffer", () => {
        var js_code = `exports.hi = v => {
            var buf = new Buffer("abcd");
            var str = "abcd"
            console.log(Buffer.isBuffer(buf));
            console.log(Buffer.isBuffer(str));
        }`;
        set_code(name, fibos.compileCode(js_code));

        var r = hi('bufb', {
            authorization: name
        });
        assert.equal(r.processed.action_traces[0].console, 'true\nfalse\n');
    });

    it("instanceof", () => {
        var js_code = `exports.hi = v => {
            var buf = new Buffer("abcd");
            console.log(buf instanceof Buffer);
            console.log(buf instanceof Uint8Array);
        }`;
        set_code(name, fibos.compileCode(js_code));

        var r = hi('buafb', {
            authorization: name
        });
        assert.equal(r.processed.action_traces[0].console, 'true\ntrue\n');
    });

    it("concat", () => {
        var js_code = `exports.hi = v => {
            var buf1 = new Buffer("abcd");
            var buf2 = new Buffer("efg");
            var buf3 = new Buffer();
            var bufArray = [buf1];
            var bufRes = Buffer.concat(bufArray);
            console.log(bufRes.toString());

            bufArray = [buf1, buf2];
            bufRes = Buffer.concat(bufArray);
            console.log(bufRes.toString());

            bufRes = Buffer.concat(bufArray, 6);
            console.log(bufRes.toString());

            buf1 = new Buffer([0x31, 0x32, 0x33, 0x34]);
            buf2 = new Buffer([0x35, 0x36, 0x37, 0x38]);
            bufArray = [buf1, buf2];
            bufRes = Buffer.concat(bufArray);
            console.log(bufRes.toString());

            var buf2 = Buffer.concat([]);
            console.log(buf2.length);
        }`;
        set_code(name, fibos.compileCode(js_code));

        var r = hi('bufh', {
            authorization: name
        });
        assert.equal(r.processed.action_traces[0].console, 'abcd\nabcdefg\nabcdef\n12345678\n0\n');
    });

    it("Buffer.from(String)", () => {
        var js_code = `exports.hi = v => {
            var buf = Buffer.from("abcd");
            console.log(buf.toString());

            var buf = new Buffer("100");
            console.log(buf.toString());
        }`;
        set_code(name, fibos.compileCode(js_code));

        var r = hi('bufg', {
            authorization: name
        });
        assert.equal(r.processed.action_traces[0].console, 'abcd\n100\n');
    });

    it("Buffer.from(String,codec & undefined)", () => {
        var js_code = `exports.hi = v => {
            var buf = Buffer.from("你好", "utf8");
            console.log(buf.toString());

            var buf = new Buffer("我好", undefined);
            console.log(buf.toString());
        }`;
        set_code(name, fibos.compileCode(js_code));

        var r = hi('bufppg', {
            authorization: name
        });
        assert.equal(r.processed.action_traces[0].console, '你好\n我好\n');
    });

    it("Buffer.from(Array)", () => {
        var js_code = `exports.hi = v => {
            var buf = Buffer.from([0x31, 0x32, 0x33, 0x34]);
            console.log(buf.toString());
        }`;
        set_code(name, fibos.compileCode(js_code));

        var r = hi('bufi', {
            authorization: name
        });
        assert.equal(r.processed.action_traces[0].console, '1234\n');
    });

    it("Buffer.from(ArrayBuffer)", () => {
        var js_code = `exports.hi = v => {
            var arr = new Uint16Array(2);
            arr[0] = 5000;
            arr[1] = 4000;
            var buf = Buffer.from(arr.buffer);
            console.log(buf.hex());
        }`;
        set_code(name, fibos.compileCode(js_code));

        var r = hi('bufn', {
            authorization: name
        });
        assert.equal(r.processed.action_traces[0].console, '8813a00f\n');
    });

    it("Buffer.from(Buffer)", () => {
        var js_code = `exports.hi = v => {
            var buf = Buffer.from(new Buffer("abcd"));
            console.log(buf.toString());
        }`;
        set_code(name, fibos.compileCode(js_code));

        var r = hi('bufo', {
            authorization: name
        });
        assert.equal(r.processed.action_traces[0].console, 'abcd\n');
    });

    it("Buffer.from(String)", () => {
        var js_code = `exports.hi = v => {
            var buf = Buffer.from("abcd");
            console.log(buf.toString());
        }`;
        set_code(name, fibos.compileCode(js_code));

        var r = hi('bufdff', {
            authorization: name
        });
        assert.equal(r.processed.action_traces[0].console, 'abcd\n');
    });

    it("Buffer.byteLength", () => {
        var js_code = `exports.hi = v => {
            var str1 = "\u00bd + \u00bc = \u00be";
            var str2 = "0xffffff";
            var str3 = "YnVmZmVy";  
            console.log(Buffer.byteLength(str1));
            console.log(Buffer.byteLength(str1, "utf8"));
            console.log(Buffer.byteLength(str2, "hex"));
            console.log(Buffer.byteLength(str3, "base64"));
        }`;
        set_code(name, fibos.compileCode(js_code));

        var r = hi('sbf', {
            authorization: name
        });
        assert.equal(r.processed.action_traces[0].console, '12\n12\n4\n6\n');
    });

    xit("Buffer.byteLength(other)", () => {
        var js_code = `exports.hi = v => {
            console.log(Buffer.byteLength({}));
            console.log(Buffer.byteLength(function () {}));
            console.log(Buffer.byteLength(() => {}));
            console.log(Buffer.byteLength([]));
        }`;
        set_code(name, fibos.compileCode(js_code));

        var r = hi('bfds', {
            authorization: name
        });
        assert.equal(r.processed.action_traces[0].console, '15\n14\n8\n0\n');
    });

    it("Buffer.alloc(Integer)", () => {
        var js_code = `exports.hi = v => {
            var buf1 = Buffer.alloc(10, 2);
            var buf2 = Buffer.alloc(2, 0xf);
            console.log(buf1.toString());
            console.log(buf2.toString());
        }`;
        set_code(name, fibos.compileCode(js_code));

        var r = hi('bfdd', {
            authorization: name
        });
        assert.equal(r.processed.action_traces[0].console, '\u0002\u0002\u0002\u0002\u0002\u0002\u0002\u0002\u0002\u0002\n\u000f\u000f\n');
    });

    it("Buffer.alloc(String)", () => {
        var js_code = `exports.hi = v => {
            var buf1 = Buffer.alloc(11, 'aGVsbG8gd29ybGQ=', 'base64');
            var buf2 = Buffer.alloc(16, 'aGVsbG8gd29ybGQ=', 'base64');
            var buf3 = Buffer.alloc(15, new Buffer('helloworld'), 'utf8');
            console.log(buf1.toString());
            console.log(buf2.toString());
            console.log(buf3.toString());
        }`;
        set_code(name, fibos.compileCode(js_code));

        var r = hi('bdds', {
            authorization: name
        });
        assert.equal(r.processed.action_traces[0].console, 'hello world\nhello worldhello\nhelloworldhello\n');
    });

    it("Buffer.allocUnsafe(Integer)", () => {
        var js_code = `exports.hi = v => {
            var buf1 = Buffer.allocUnsafe(10);
            console.log(buf1.length);
        }`;
        set_code(name, fibos.compileCode(js_code));

        var r = hi('bddjs', {
            authorization: name
        });
        assert.equal(r.processed.action_traces[0].console, '10\n');
    });

    it("Buffer.allocUnsafeSlow(Integer)", () => {
        var js_code = `exports.hi = v => {
            var buf1 = Buffer.allocUnsafeSlow(10);
            console.log(buf1.length);
        }`;
        set_code(name, fibos.compileCode(js_code));

        var r = hi('bddsd', {
            authorization: name
        });
        assert.equal(r.processed.action_traces[0].console, '10\n');
    });

    it("Buffer.isEncoding", () => {
        var js_code = `exports.hi = v => {
            console.log(Buffer.isEncoding('utf8')); // true
            console.log(Buffer.isEncoding('utf-8')); // true
            console.log(Buffer.isEncoding('gbk')); // false
            console.log(Buffer.isEncoding('gb2312')); // false
            console.log(Buffer.isEncoding('hex')); // true
            console.log(Buffer.isEncoding('base64')); // true
            console.log(Buffer.isEncoding('jis')); // false
            console.log(Buffer.isEncoding('aaabbbccc')); // false
            console.log(Buffer.isEncoding('binary')); // true
            console.log(Buffer.isEncoding('latin1')); // true
            console.log(Buffer.isEncoding('big5')); // false
        }`;
        set_code(name, fibos.compileCode(js_code));

        var r = hi('bdded', {
            authorization: name
        });
        assert.equal(r.processed.action_traces[0].console, 'true\ntrue\nfalse\nfalse\ntrue\ntrue\nfalse\nfalse\ntrue\ntrue\nfalse\n');
    });

    it("keys", () => {
        var js_code = `exports.hi = v => {
            var buf1 = new Buffer("buffer");
            var keys1 = [];
            for (let key of buf1.keys()) {
                keys1.push(key);
            }
            console.log(keys1.toString());
        }`;
        set_code(name, fibos.compileCode(js_code));

        var r = hi('bo', {
            authorization: name
        });
        assert.equal(r.processed.action_traces[0].console, '0,1,2,3,4,5\n');
    });

    it("values", () => {
        var js_code = `exports.hi = v => {
            var buf1 = new Buffer("buffer");
            var values1 = [];
            for (let value of buf1.values()) {
                values1.push(value);
            }
            console.log(values1.toString());
        }`;
        set_code(name, fibos.compileCode(js_code));

        var r = hi('bop', {
            authorization: name
        });
        assert.equal(r.processed.action_traces[0].console, '98,117,102,102,101,114\n');
    });

    it("entries", () => {
        var js_code = `exports.hi = v => {
            var buf1 = new Buffer("buffer");
            var entries1 = [];
            for (let value of buf1.entries()) {
                entries1.push(value);
            }
            console.log(entries1.toString());
        }`;
        set_code(name, fibos.compileCode(js_code));

        var r = hi('bope', {
            authorization: name
        });
        assert.equal(r.processed.action_traces[0].console, '0,98,1,117,2,102,3,102,4,101,5,114\n');
    });

    it("toArray", () => {
        var js_code = `exports.hi = v => {
            var buf = new Buffer("buffer");
            console.log(buf.toArray());
        }`;
        set_code(name, fibos.compileCode(js_code));

        var r = hi('bopded', {
            authorization: name
        });
        assert.equal(r.processed.action_traces[0].console, '[\n  98,\n  117,\n  102,\n  102,\n  101,\n  114\n]\n');
    });

    it("toString", () => {
        var js_code = `exports.hi = v => {
            var buf = new Buffer([0x31, 0x32, 0x33, 0x34]);
            console.log(buf.toString("utf8"));
            console.log(buf.toString(undefined));
            console.log(buf.toString("hex"));
            console.log(buf.toString("base64"));
            console.log(buf.toString("utf8", 1));
            console.log(buf.toString("utf8", 1, 3));
            console.log(buf.toString("hex", 2));
            console.log(buf.toString("base64", 2));
        }`;
        set_code(name, fibos.compileCode(js_code));

        var r = hi('bo3s', {
            authorization: name
        });
        assert.equal(r.processed.action_traces[0].console, '1234\n1234\n31323334\nMTIzNA==\n234\n23\n3334\nMzQ=\n');
    });

    it("write", () => {
        var js_code = `exports.hi = v => {
            var buf = new Buffer(10);
            buf.write("abcd", 0, 4)
            console.log(buf.toString('utf8', 0, 4));

            buf = new Buffer(10);
            buf.write("MTIzNA==", 0, 4, "base64");
            console.log(buf.toString("utf8", 0, 4));
        }`;
        set_code(name, fibos.compileCode(js_code));

        var r = hi('bos5s2d', {
            authorization: name
        });
        assert.equal(r.processed.action_traces[0].console, 'abcd\n1234\n');
    });

    it("fill", () => {
        var js_code = `exports.hi = v => {
            var buf = new Buffer(10);
            buf.fill("abc");
            console.log(buf.toString());

            buf.fill("cbacbacbacba");
            console.log(buf.toString());
        }`;
        set_code(name, fibos.compileCode(js_code));

        var r = hi('bosp5s2d', {
            authorization: name
        });
        assert.equal(r.processed.action_traces[0].console, 'abcabcabca\ncbacbacbac\n');
    });

    it('fill zero', () => {
        var js_code = `exports.hi = v => {
                var buf = new Buffer(10);
                console.log(buf);
            }`;
        set_code(name, fibos.compileCode(js_code));

        var r = hi('safebuf', {
            authorization: name
        });

        assert.equal(r.processed.action_traces[0].console, '<Buffer 00 00 00 00 00 00 00 00 00 00>\n');
    });

    it("slice", () => {
        var js_code = `exports.hi = v => {
            var buf = new Buffer(10);
            buf.write("abcdefghih");
            console.log(buf.slice(0, 3).toString());
            console.log(buf.slice(0, 11).toString());
            console.log(buf.slice(8).toString());
        }`;
        set_code(name, fibos.compileCode(js_code));

        var r = hi('bol2d', {
            authorization: name
        });
        assert.equal(r.processed.action_traces[0].console, 'abc\nabcdefghih\nih\n');
    });

    it("equals & compare", () => {
        var js_code = `exports.hi = v => {
            var buf = new Buffer("abcd");
            console.log(buf.equals(new Buffer("abcd")));
            console.log(buf.equals(new Buffer("abc")));
            console.log(buf.compare(new Buffer("abcd")));
            console.log(buf.compare(new Buffer("abc")));
            console.log(buf.compare(new Buffer("abcde")));
        }`;
        set_code(name, fibos.compileCode(js_code));

        var r = hi('bogl2d', {
            authorization: name
        });
        assert.equal(r.processed.action_traces[0].console, 'true\nfalse\n0\n1\n-1\n');
    });

    it("copy", () => {
        var js_code = `exports.hi = v => {
            var buf1 = new Buffer([0x31, 0x32, 0x33]);
            var arr = [0x34, 0x35, 0x36];

            var buf2 = new Buffer(arr);
            var sz = buf1.copy(buf2);
            console.log(sz);
            console.log(buf2.toString());
        }`;
        set_code(name, fibos.compileCode(js_code));

        var r = hi('bohgl2d', {
            authorization: name
        });
        assert.equal(r.processed.action_traces[0].console, '3\n123\n');
    });

    it("codec", () => {
        var js_code = `exports.hi = v => {
            console.log(new Buffer("哈哈哈").toString());
            console.log(new Buffer("哈哈哈哈", "gbk").toArray());
            console.log(new Buffer("哈哈哈", "gbk").toString("gbk"));
        }`;
        set_code(name, fibos.compileCode(js_code));

        assert.throws(() => {
            hi('bohgfl2d', {
                authorization: name
            });
        });
    });

    it("indexOf", () => {
        var js_code = `exports.hi = v => {
            var buf = new Buffer([0x31, 0x32, 0x33, 0x34, 0x00]);
            console.log(buf.indexOf(0x33));
            console.log(buf.indexOf(0x00));
        }`;
        set_code(name, fibos.compileCode(js_code));

        var r = hi('bohj2d', {
            authorization: name
        });
        assert.equal(r.processed.action_traces[0].console, '2\n4\n');
    });

    it("indexOf out of range", () => {
        var js_code = `exports.hi = v => {
            var buf = new Buffer("cacdbfcde");
            buf.indexOf("cd", 10);
        }`;
        set_code(name, fibos.compileCode(js_code));

        assert.throws(() => {
            hi('bohj3d', {
                authorization: name
            });
        });
    });

    it("reverse", () => {
        var js_code = `exports.hi = v => {
           var a = new Buffer("abcd");
           console.log(a.reverse().toString());
        }`;
        set_code(name, fibos.compileCode(js_code));

        var r = hi('bohhsj2d', {
            authorization: name
        });
        assert.equal(r.processed.action_traces[0].console, 'dcba\n');
    });

    it("join", () => {
        var js_code = `exports.hi = v => {
            var a = new Buffer([192, 168, 0, 1]);
            console.log(a.join('.'));
        }`;
        set_code(name, fibos.compileCode(js_code));

        var r = hi('bohxhsj2d', {
            authorization: name
        });
        assert.equal(r.processed.action_traces[0].console, '192.168.0.1\n');
    });
});

require.main === module && test.run(console.DEBUG);