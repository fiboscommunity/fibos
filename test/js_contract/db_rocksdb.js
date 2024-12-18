var test = require('test');
test.setup();

var FIBOS = require('@tpblock/client')
var test_util = require('./test_util');
var coroutine = require('coroutine');

describe('db', () => {
    var name;
    var name1;
    var ctx;
    var fibos;
    var db_abi = {
        "version": "eosio::abi/1.0",
        "types": [{
            "new_type_name": "my_account_name",
            "type": "name"
        }],
        "structs": [{
            "name": "player",
            "base": "",
            "fields": [{
                "name": "title",
                "type": "string"
            }, {
                "name": "age",
                "type": "int32"
            }]
        }, {
            "name": "player2",
            "base": "",
            "fields": [{
                "name": "title",
                "type": "string"
            }, {
                "name": "age",
                "type": "int32"
            }, {
                "name": "weight",
                "type": "int32"
            }, {
                "name": "length",
                "type": "int32"
            }, {
                "name": "width",
                "type": "int32"
            }]
        }, {
            "name": "player3",
            "base": "",
            "fields": [{
                "name": "id",
                "type": "int64"
            }, {
                "name": "name",
                "type": "name"
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
                "type": "int64"
            }]
        }, {
            "name": "hi2",
            "base": "",
            "fields": [{
                "name": "user",
                "type": "int64"
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
        }, {
            "name": "hi2",
            "type": "hi2",
            "ricardian_contract": ""
        }],
        "tables": [{
            "name": "players",
            "type": "player",
            "index_type": "i64",
            "key_names": ["nickname"],
            "key_types": ["my_account_name"]
        }, {
            "name": "players1",
            "type": "player",
            "index_type": "i64",
            "key_names": ["id"],
            "key_types": ["int64"]
        }, {
            "name": "players2",
            "type": "player2",
            "index_type": "i64",
            "key_names": ["id"],
            "key_types": ["int64"]
        }, {
            "name": "players3",
            "type": "player2",
            "index_type": "i64",
            "key_names": ["id"],
            "key_types": ["int64"]
        }, {
            "name": "players4",
            "type": "player2",
            "index_type": "i64",
            "key_names": ["id"],
            "key_types": ["int64"]
        }, {
            "name": "players5",
            "type": "player2",
            "index_type": "i64",
            "key_types": ["int64"]
        }, {
            "name": "players11",
            "type": "player2",
            "index_type": "i64",
            "key_names": ["id"]
        }, {
            "name": "players12",
            "type": "player2",
            "index_type": "i64",
            "key_names": ["id"],
            "key_types": ["int64"]
        }, {
            "name": "players13",
            "type": "player3",
            "index_type": "i64",
            "key_names": ["id"],
            "key_types": ["int64"]
        }]
    };

    before(() => {
        require.main === module && test_util.node("rocks");
        fibos = FIBOS(test_util.config);
        name = test_util.user(fibos);
        name1 = test_util.user(fibos);

        fibos.setabiSync(name, db_abi);
        fibos.setabiSync(name1, db_abi);
    });
    after(() => {
        require.main === module && test_util.stop();
    });

    it('list tables', () => {
        var js_code = `exports.hi = v => console.error(Object.keys(db));`;
        fibos.setcodeSync(name, 0, 0, fibos.compileCode(js_code));

        ctx = fibos.contractSync(name);

        var r = ctx.hiSync('lion1234', {
            authorization: name
        });
        assert.equal(r.processed.action_traces[0].console, "[\n  \"players\",\n  \"players1\",\n  \"players2\",\n  \"players3\",\n  \"players4\",\n  \"players5\",\n  \"players11\",\n  \"players12\",\n  \"players13\"\n]\n");
    });

    it('table info', () => {
        var js_code = `var players = db.players("code", "scope");exports.hi = v => console.error(players.name, players.code, players.scope);`;
        fibos.setcodeSync(name, 0, 0, fibos.compileCode(js_code));

        ctx = fibos.contractSync(name);

        var r = ctx.hiSync('info', {
            authorization: name
        });
        assert.equal(r.processed.action_traces[0].console, 'players code scope\n');
    });

    it('emplace', () => {
        var js_code = `exports.hi = v => {
            var players = db.players(action.account, action.account);
            players.emplace(action.account, {title: "ceo",age:48,nickname:"lion1"});}`;
        fibos.setcodeSync(name, 0, 0, fibos.compileCode(js_code));

        ctx = fibos.contractSync(name);

        var r = ctx.hiSync('emplace', {
            authorization: name
        });

        assert.deepEqual(fibos.getTableRowsSync({
            json: true,
            scope: name,
            code: name,
            table: "players"
        }), {
            "rows": [{
                "title": "ceo",
                "age": 48
            }],
            "more": false,
            "next_key": "",
            "next_key_bytes": ""
        });
    });

    it('emplace by int64', () => {
        var js_code = `exports.hi = v => {
            var players = db.players1(action.account, action.account);
            players.emplace(action.account, {title: "ceo1",age:47,id:123n});
            players.emplace(action.account, {title: "ceo2",age:50,id:456n});}`;
        fibos.setcodeSync(name, 0, 0, fibos.compileCode(js_code));

        ctx = fibos.contractSync(name);

        var r = ctx.hiSync('emplace1', {
            authorization: name
        });

        assert.deepEqual(fibos.getTableRowsSync({
            json: true,
            scope: name,
            code: name,
            table: "players1"
        }), {
            "rows": [{
                "title": "ceo1",
                "age": 47
            }, {
                "title": "ceo2",
                "age": 50
            }],
            "more": false,
            "next_key": "",
            "next_key_bytes": ""
        });
    });

    describe('find', () => {
        it('find', () => {
            var js_code = `exports.hi = v => {
                var players = db.players(action.account, action.account);
                console.log(players.find(v).data);}`;
            fibos.setcodeSync(name, 0, 0, fibos.compileCode(js_code));

            ctx = fibos.contractSync(name);

            var r = ctx.hiSync("lion1", {
                authorization: name
            });

            assert.equal(r.processed.action_traces[0].console, '{\n  \"title\": \"ceo\",\n  \"age\": 48n\n}\n');
        });

        it('find by int64', () => {
            var js_code = `exports.hi1 = v => {
                var players = db.players1(action.account, action.account);
                console.log(players.find(v).data);}`;
            fibos.setcodeSync(name, 0, 0, fibos.compileCode(js_code));

            ctx = fibos.contractSync(name);

            var r = ctx.hi1Sync(123, {
                authorization: name
            });

            assert.equal(r.processed.action_traces[0].console, '{\n  \"title\": \"ceo1\",\n  \"age\": 47n\n}\n');
        });

        it('not found', () => {
            var js_code = `exports.hi1 = v => {
                var players = db.players(action.account, action.account);
                var itr = players.find(v);
                console.log(itr.is_end());}`;
            fibos.setcodeSync(name, 0, 0, fibos.compileCode(js_code));

            ctx = fibos.contractSync(name);

            var r = ctx.hi1Sync(1234, {
                authorization: name
            });

            assert.equal(r.processed.action_traces[0].console, 'true\n');
        });
    });

    it('lowerbound/upperbound', () => {
        var js_code = `exports.hi1 = v => {
            var players = db.players1(action.account, action.account);
            var itr = players.lowerbound(123);
            var itr1 = players.upperbound(123);
            console.log(itr.data, itr1.data);}`;
        fibos.setcodeSync(name, 0, 0, fibos.compileCode(js_code));

        ctx = fibos.contractSync(name);

        var r = ctx.hi1Sync(323, {
            authorization: name
        });

        assert.equal(r.processed.action_traces[0].console, '{\n  \"title\": \"ceo1\",\n  \"age\": 47n\n} {\n  \"title\": \"ceo2\",\n  \"age\": 50n\n}\n');
    });

    it('next', () => {
        var js_code = `exports.hi1 = v => {
            var players = db.players1(action.account, action.account);
            var itr = players.find(v);
            var itr1 = itr.next();
            var itr2 = itr1.next();
            console.log(itr.is_begin(), itr1.data, itr2.is_end());}`;
        fibos.setcodeSync(name, 0, 0, fibos.compileCode(js_code));

        ctx = fibos.contractSync(name);

        var r = ctx.hi1Sync(123, {
            authorization: name
        });

        assert.equal(r.processed.action_traces[0].console, 'true {\n  \"title\": \"ceo2\",\n  \"age\": 50n\n} true\n');
    });

    it('previous', () => {
        var js_code = `exports.hi1 = v => {
            var players = db.players1(action.account, action.account);
            var itr = players.find(v);
            var itr1 = itr.previous();
            console.log(itr1.data, itr1.is_begin());}`;
        fibos.setcodeSync(name, 0, 0, fibos.compileCode(js_code));

        ctx = fibos.contractSync(name);

        var r = ctx.hi1Sync(456, {
            authorization: name
        });

        assert.equal(r.processed.action_traces[0].console, '{\n  \"title\": \"ceo1\",\n  \"age\": 47n\n} true\n');
    });

    it('update', () => {
        var js_code = `exports.hi = v => {
            var players = db.players1(action.account, action.account);
            var itr = players.find(123);
            itr.data.title = 'cto';
            itr.data.age = 23;
            itr.update(action.account);}`;
        fibos.setcodeSync(name, 0, 0, fibos.compileCode(js_code));

        ctx = fibos.contractSync(name);

        var r = ctx.hiSync('12311', {
            authorization: name
        });

        assert.deepEqual(fibos.getTableRowsSync({
            json: true,
            scope: name,
            code: name,
            table: "players1"
        }), {
            "rows": [{
                "title": "cto",
                "age": 23
            },
            {
                "title": "ceo2",
                "age": 50
            }
            ],
            "more": false,
            "next_key": "",
            "next_key_bytes": ""
        });
    });

    it('remove', () => {
        var js_code = `exports.hi = v => {
            var players = db.players1(action.account, action.account);
            var itr = players.find(123);
            itr.remove();
            var itr1 = players.find(123);
            console.log(itr1.data, itr1.is_end())
        }`;
        fibos.setcodeSync(name, 0, 0, fibos.compileCode(js_code));

        ctx = fibos.contractSync(name);

        var r = ctx.hiSync('', {
            authorization: name
        });
        assert.equal(r.processed.action_traces[0].console, 'undefined true\n');

        assert.deepEqual(fibos.getTableRowsSync({
            json: true,
            scope: name,
            code: name,
            table: "players1"
        }), {
            "rows": [{
                "title": "ceo2",
                "age": 50
            }],
            "more": false,
            "next_key": "",
            "next_key_bytes": ""
        });
    });

    describe('indexes', () => {
        it('declare', () => {
            var js_code = `
            const indexes = {
                age: [64, o => [o.age]]
            };
            exports.hi = v => {
                var players = db.players(action.account, action.account, indexes);
            }`;
            fibos.setcodeSync(name, 0, 0, fibos.compileCode(js_code));

            ctx = fibos.contractSync(name);

            var r = ctx.hiSync('ddd', {
                authorization: name
            });
        });

        it('name too lang', () => {
            var js_code = `
            const indexes = {
                age1234512345: [64, o => [o.age]]
            };
            exports.hi = v => {
                var players = db.players(action.account, action.account, indexes);
            }`;
            fibos.setcodeSync(name, 0, 0, fibos.compileCode(js_code));

            ctx = fibos.contractSync(name);

            assert.throws(() => {
                var r = ctx.hiSync('1122', {
                    authorization: name
                });
            });
        });

        it('emplace', () => {
            var js_code = `
            const indexes = {
                age: [64, o => [o.age]]
            };
            exports.hi = v => {
                var players = db.players(action.account, action.account, indexes);
                players.emplace(action.account, {title: "ceo",age:48,nickname:"lion2"});
            }`;
            fibos.setcodeSync(name, 0, 0, fibos.compileCode(js_code));

            ctx = fibos.contractSync(name);

            var r = ctx.hiSync('store', {
                authorization: name
            });

            assert.deepEqual(fibos.getTableRowsSync({
                json: true,
                scope: name,
                code: name,
                table: "players"
            }), {
                "rows": [{
                    "title": "ceo",
                    "age": 48
                }, {
                    "title": "ceo",
                    "age": 48
                }],
                "more": false,
                "next_key": "",
                "next_key_bytes": ""
            });
        });

        it('find', () => {
            var js_code = `
            const indexes = {
                age: [64, o => [o.age]]
            };
            exports.hi = v => {
                var players = db.players(action.account, action.account, indexes);
                var itr = players.indexes.age.find({age:48});
                console.log(itr.data);
                var itr1 = players.indexes.age.find({age:49});
                console.log(itr1.is_end());
            }`;
            fibos.setcodeSync(name, 0, 0, fibos.compileCode(js_code));

            ctx = fibos.contractSync(name);

            var r = ctx.hiSync('find', {
                authorization: name
            });

            assert.equal(r.processed.action_traces[0].console, '{\n  \"title\": \"ceo\",\n  \"age\": 48n\n}\ntrue\n');
        });

        it('update', () => {
            var js_code = `
            const indexes = {
                age: [64, o => [o.age]]
            };
            exports.hi = v => {
                var players = db.players(action.account, action.account, indexes);
                var itr = players.indexes.age.find({age:48});
                itr.data.age = 49;
                itr.update(action.account);
                var itr1 = players.indexes.age.find({age:49});
                console.log(itr1.data, itr1.is_end());
                var itr2 = players.indexes.age.find({age:48});
                console.log(itr2.is_end());
            }`;
            fibos.setcodeSync(name, 0, 0, fibos.compileCode(js_code));

            ctx = fibos.contractSync(name);

            var r = ctx.hiSync('update', {
                authorization: name
            });

            assert.equal(r.processed.action_traces[0].console, '{\n  \"title\": \"ceo\",\n  \"age\": 49n\n} false\ntrue\n');
        });

        it('find secondery key', () => {
            var js_code = `
            const indexes = {
                age: [64, o => [o.age]]
            };
            exports.hi = v => {
                var players = db.players(action.account, action.account, indexes);
                players.emplace(action.account, {title: "ceo1",age:50,nickname:"tiger"});
            }`;
            fibos.setcodeSync(name, 0, 0, fibos.compileCode(js_code));

            ctx = fibos.contractSync(name);

            var r = ctx.hiSync('store', {
                authorization: name
            });

            assert.deepEqual(fibos.getTableRowsSync({
                json: true,
                scope: name,
                code: name,
                table: "players",
                key_type: "i64",
                index_position: 2,
                lower_bound: 0
            }), {
                "rows": [{
                    "title": "ceo",
                    "age": 49
                }, {
                    "title": "ceo1",
                    "age": 50
                }],
                "more": false,
                "next_key": "",
                "next_key_bytes": ""
            });

            assert.deepEqual(fibos.getTableRowsSync({
                json: true,
                scope: name,
                code: name,
                table: "players",
                key_type: "i64",
                index_position: 2,
                lower_bound: 50
            }), {
                "rows": [{
                    "title": "ceo1",
                    "age": 50
                }],
                "more": false,
                "next_key": "",
                "next_key_bytes": ""
            });

            var js_code = `
            const indexes = {
                age: [64, o => [o.age]]
            };
            exports.hi = v => {
                var players = db.players(action.account, action.account, indexes);
                var itr = players.indexes.age.find({age:50});
                itr.remove();
                var itr1 = players.indexes.age.find({age:50});
                console.log(itr1.data, itr1.is_end());
            }`;
            fibos.setcodeSync(name, 0, 0, fibos.compileCode(js_code));

            ctx = fibos.contractSync(name);

            var r = ctx.hiSync('remove', {
                authorization: name
            });
        });

        it('remove', () => {
            var js_code = `
            const indexes = {
                age: [64, o => [o.age]]
            };
            exports.hi = v => {
                var players = db.players(action.account, action.account, indexes);
                var itr = players.indexes.age.find({age:49});
                itr.remove();
                var itr1 = players.indexes.age.find({age:49});
                console.log(itr1.data, itr1.is_end());
            }`;
            fibos.setcodeSync(name, 0, 0, fibos.compileCode(js_code));

            ctx = fibos.contractSync(name);

            var r = ctx.hiSync('remove1', {
                authorization: name
            });

            assert.equal(r.processed.action_traces[0].console, 'undefined true\n');
        });
    });

    describe('indexes Multi field', () => {
        it('declare', () => {
            var js_code = `
                const indexes = {
                    detail:[128, o => [o.age,o.weight]]
                };
                exports.hi2 = v => {
                    var players2 = db.players2(action.account, action.account, indexes);
                }`;
            fibos.setcodeSync(name, 0, 0, fibos.compileCode(js_code));

            ctx = fibos.contractSync(name);

            var r = ctx.hi2Sync(11, {
                authorization: name
            });
        });

        it('name too lang', () => {
            var js_code = `
            const indexes = {
                age1234512345: [128, o => [o.age,o.weight]]
            };
            exports.hi2 = v => {
                var players2 = db.players2(action.account, action.account, indexes);
            }`;
            fibos.setcodeSync(name, 0, 0, fibos.compileCode(js_code));

            ctx = fibos.contractSync(name);

            assert.throws(() => {
                var r = ctx.hi2Sync(12, {
                    authorization: name
                });
            });
        });

        it('emplace', () => {
            var js_code = `
            const indexes = {
                detail: [128, o => [o.age,o.weight]]
            };
            exports.hi2 = v => {
                var players2 = db.players2(action.account, action.account, indexes);
                players2.emplace(action.account, {title:"ceo",age:48,weight:100,length:170,width:100,id:1});
            }`;
            fibos.setcodeSync(name, 0, 0, fibos.compileCode(js_code));

            ctx = fibos.contractSync(name);

            var r = ctx.hi2Sync(13, {
                authorization: name
            });

            assert.deepEqual(fibos.getTableRowsSync({
                json: true,
                scope: name,
                code: name,
                table: "players2"
            }), {
                "rows": [{
                    "title": "ceo",
                    "age": 48,
                    "weight": 100,
                    "length": 170,
                    "width": 100
                }],
                "more": false,
                "next_key": "",
                "next_key_bytes": ""
            });
        });

        describe('find', () => {
            it('find', () => {
                var js_code = `
                        const indexes = {
                            detail: [128, o => [o.age,o.weight]]
                        };
                        exports.hi2 = v => {
                            var players2 = db.players2(action.account, action.account, indexes);
                            var itr = players2.indexes.detail.find({age:48,weight:100});
                            console.log(itr.data);
                            var itr1 = players2.indexes.detail.find({age:49,weight:100});
                            console.log(itr1.is_end());
                        }`;
                fibos.setcodeSync(name, 0, 0, fibos.compileCode(js_code));

                ctx = fibos.contractSync(name);

                var r = ctx.hi2Sync(14, {
                    authorization: name
                });
                assert.equal(r.processed.action_traces[0].console, '{\n  \"title\": \"ceo\",\n  \"age\": 48n,\n  \"weight\": 100n,\n  \"length\": 170n,\n  \"width\": 100n\n}\ntrue\n');
            });

            it('missing primary key', () => {
                var js_code = `
                const indexes = {
                    detail: [128, o => [o.age,o.weight]]
                };
                exports.hi2 = v => {
                    var players2 = db.players2(action.account, action.account, indexes);
                    var itr = players2.indexes.detail.find({age:48});
                }`;
                fibos.setcodeSync(name, 0, 0, fibos.compileCode(js_code));

                ctx = fibos.contractSync(name);

                assert.throws(() => {
                    var r = ctx.hi2Sync(1122, {
                        authorization: name
                    });
                });
            })
        })

        it('update', () => {
            var js_code = `
            const indexes = {
                detail: [128, o => [o.age,o.weight]]
            };
            exports.hi2 = v => {
                var players2 = db.players2(action.account, action.account, indexes);
                var itr = players2.indexes.detail.find({age:48,weight:100});
                itr.data.age = 18;
                itr.update(action.account);
                var itr1 = players2.indexes.detail.find({age:18,weight:100});
                console.log(itr1.data, itr1.is_end());
                var itr2 = players2.indexes.detail.find({age:48,weight:100});
                console.log(itr2.is_end());
            }`;
            fibos.setcodeSync(name, 0, 0, fibos.compileCode(js_code));

            ctx = fibos.contractSync(name);

            var r = ctx.hi2Sync(15, {
                authorization: name
            });

            assert.equal(r.processed.action_traces[0].console, '{\n  \"title\": \"ceo\",\n  \"age\": 18n,\n  \"weight\": 100n,\n  \"length\": 170n,\n  \"width\": 100n\n} false\ntrue\n');
        });

        it('remove', () => {
            var js_code = `
            const indexes = {
                detail: [128, o => [o.age,o.weight]]
            };
            exports.hi2 = v => {
                var players2 = db.players2(action.account, action.account, indexes);
                var itr = players2.indexes.detail.find({age:18,weight:100});
                itr.remove();
                var itr1 = players2.indexes.detail.find({age:19,weight:100});
                console.log(itr1.data, itr1.is_end());
            }`;
            fibos.setcodeSync(name, 0, 0, fibos.compileCode(js_code));

            ctx = fibos.contractSync(name);

            var r = ctx.hi2Sync(16, {
                authorization: name
            });

            assert.equal(r.processed.action_traces[0].console, 'undefined true\n');
        });
    });

    describe('indexes Multi indexes', () => {
        it('decare', () => {
            var js_code = `
            const indexes = {
                age:[64, o=>[o.age]],
                detail1:[128, o => [o.age,o.weight]],
                detail2:[192, o => [o.age,o.weight,o.length]],
                detail3:[256, o => [o.age,o.weight,o.length,o.width]]
            };
            exports.hi2 = v => {
                var players = db.players2(action.account, action.account, indexes);
            }`;
            fibos.setcodeSync(name, 0, 0, fibos.compileCode(js_code));

            ctx = fibos.contractSync(name);

            var r = ctx.hi2Sync(1, {
                authorization: name
            });
        });

        it('name too lang', () => {
            var js_code = `
            const indexes = {
                age:[64, o=>[o.age]],
                detail1:[128, o => [o.age,o.weight]],
                detail2:[192, o => [o.age,o.weight,o.length]],
                detail3:[256, o => [o.age,o.weight,o.length,o.width]]
            };
            exports.hi = v => {
                var players2 = db.players2(action.account, action.account, indexes);
            }`;
            fibos.setcodeSync(name, 0, 0, fibos.compileCode(js_code));

            ctx = fibos.contractSync(name);

            assert.throws(() => {
                var r = ctx.hi2Sync(1, {
                    authorization: name
                });
            });
        });

        it('size of index key out of range', () => {
            var js_code = `
            const indexes = {
                detail1:[320, o => [o.age,o.weight,o.length,o.width,o.width]]
            };
            exports.hi2 = v => {
                var players2 = db.players2(action.account, action.account, indexes);
            }`;
            fibos.setcodeSync(name, 0, 0, fibos.compileCode(js_code));

            ctx = fibos.contractSync(name);

            assert.throws(() => {
                ctx.hi2Sync(1, {
                    authorization: name
                });
            });
        });

        it('size of index key not a multiple of 64', () => {
            var js_code = `
            const indexes = {
                detail1:[255, o => [o.age,o.weight,o.length,o.width]]
            };
            exports.hi2 = v => {
                var players2 = db.players2(action.account, action.account, indexes);
            }`;
            fibos.setcodeSync(name, 0, 0, fibos.compileCode(js_code));

            ctx = fibos.contractSync(name);

            assert.throws(() => {
                ctx.hi2Sync(1, {
                    authorization: name
                });
            });
        });

        it('emplace', () => {
            var js_code = `
            const indexes = {
                age:[64, o=>[o.age]],
                detail1:[128, o => [o.age,o.weight]],
                detail2:[192, o => [o.age,o.weight,o.length]],
                detail3:[256, o => [o.age,o.weight,o.length,o.width]]
            };
            exports.hi2 = v => {
                var players2 = db.players2(action.account, action.account, indexes);
                players2.emplace(action.account, {title:"ceo",age:48,weight:100,length:170,width:100,id:2});
                players2.emplace(action.account, {title:"cto",age:49,weight:101,length:171,width:101,id:3});
            }`;
            fibos.setcodeSync(name, 0, 0, fibos.compileCode(js_code));

            ctx = fibos.contractSync(name);

            var r = ctx.hi2Sync(2, {
                authorization: name
            });

            assert.deepEqual(fibos.getTableRowsSync({
                json: true,
                scope: name,
                code: name,
                table: "players2"
            }), {
                "rows": [{
                    "title": "ceo",
                    "age": 48,
                    "weight": 100,
                    "length": 170,
                    "width": 100
                }, {
                    "title": "cto",
                    "age": 49,
                    "weight": 101,
                    "length": 171,
                    "width": 101
                }],
                "more": false,
                "next_key": "",
                "next_key_bytes": ""
            });
        });

        describe('find', () => {
            it('find', () => {
                var js_code = `
                const indexes = {
                    age:[64, o=>[o.age]],
                    detail1:[128, o => [o.age,o.weight]],
                    detail2:[192, o => [o.age,o.weight,o.length]],
                    detail3:[256, o => [o.age,o.weight,o.length,o.width]]
                };
                exports.hi2 = v => {
                    var players2 = db.players2(action.account, action.account, indexes);
                    var itr = players2.indexes.age.find({age:48});
                    console.log(itr.data);
                    var itr1 = players2.indexes.detail1.find({age:49,weight:101});
                    console.log(itr1.is_end());
                    var itr2 = players2.indexes.detail2.find({age:48,weight:100,length:170});
                    console.log(itr2.is_end());
                    var itr3 = players2.indexes.detail3.find({age:48,weight:100,length:170,width:100});
                    console.log(itr3.is_end());
                }`;
                fibos.setcodeSync(name, 0, 0, fibos.compileCode(js_code));

                ctx = fibos.contractSync(name);

                var r = ctx.hi2Sync(3, {
                    authorization: name
                });
                assert.equal(r.processed.action_traces[0].console, '{\n  \"title\": \"ceo\",\n  \"age\": 48n,\n  \"weight\": 100n,\n  \"length\": 170n,\n  \"width\": 100n\n}\nfalse\nfalse\nfalse\n');
            });

            it('not find', () => {
                var js_code = `
                const indexes = {
                    age:[64, o=>[o.age]],
                    detail1:[128, o => [o.age,o.weight]],
                    detail2:[192, o => [o.age,o.weight,o.length]],
                    detail3:[256, o => [o.age,o.weight,o.length,o.width]]
                };
                exports.hi2 = v => {
                    var players2 = db.players2(action.account, action.account, indexes);
                    var itr = players2.indexes.age.find({age:18});
                    console.log(itr.is_end());
                    var itr1 = players2.indexes.detail1.find({age:18,weight:100});
                    console.log(itr1.is_end());
                    var itr1_1 = players2.indexes.detail1.find({age:48,weight:101});
                    console.log(itr1_1.is_end());
                    var itr2 = players2.indexes.detail2.find({age:48,weight:100,length:171});
                    console.log(itr2.is_end());
                    var itr3 = players2.indexes.detail3.find({age:48,weight:100,length:170,width:101});
                    console.log(itr3.is_end());
                }`;
                fibos.setcodeSync(name, 0, 0, fibos.compileCode(js_code));

                ctx = fibos.contractSync(name);

                var r = ctx.hi2Sync(7, {
                    authorization: name
                });
                assert.equal(r.processed.action_traces[0].console, 'true\ntrue\ntrue\ntrue\ntrue\n');
            })
        })

        describe('next', () => {
            it('next for 64bit', () => {
                var js_code = `
                const indexes = {
                    age:[64, o=>[o.age]],
                    detail1:[128, o => [o.age,o.weight]],
                    detail2:[192, o => [o.age,o.weight,o.length]],
                    detail3:[256, o => [o.age,o.weight,o.length,o.width]]
                };
                exports.hi1 = v => {
                    var players2 = db.players2(action.account, action.account, indexes);
                    players2.emplace(action.account, {title:"coo",age:48,weight:101,length:171,width:101,id:4});
                    players2.emplace(action.account, {title:"cfo",age:49,weight:101,length:172,width:102,id:5});
                    players2.emplace(action.account, {title:"cio",age:48,weight:102,length:170,width:103,id:6});
                    players2.emplace(action.account, {title:"cso",age:50,weight:100,length:170,width:100,id:7});
                };
                exports.hi2 = v => {
                    var players2 = db.players2(action.account, action.account, indexes);
                    var itr = players2.indexes.age.find({age:49});

                    var is_end = itr.is_end();
                    while(!is_end) {
                        console.log(itr.data);
                        itr = itr.next();
                        is_end = itr.is_end();
                    }
                }`;
                fibos.setcodeSync(name, 0, 0, fibos.compileCode(js_code));

                ctx = fibos.contractSync(name);

                var r = ctx.hi1Sync(1, {
                    authorization: name
                });
                assert.equal(r.processed.action_traces[0].console, '');

                r = ctx.hi2Sync(1, {
                    authorization: name
                });
                assert.equal(r.processed.action_traces[0].console, "{\n  \"title\": \"cto\",\n  \"age\": 49n,\n  \"weight\": 101n,\n  \"length\": 171n,\n  \"width\": 101n\n}\n{\n  \"title\": \"cfo\",\n  \"age\": 49n,\n  \"weight\": 101n,\n  \"length\": 172n,\n  \"width\": 102n\n}\n{\n  \"title\": \"cso\",\n  \"age\": 50n,\n  \"weight\": 100n,\n  \"length\": 170n,\n  \"width\": 100n\n}\n");
            });

            it('next for 128bit', () => {
                var js_code = `
                const indexes = {
                    age:[64, o=>[o.age]],
                    detail1:[128, o => [o.age,o.weight]],
                    detail2:[192, o => [o.age,o.weight,o.length]],
                    detail3:[256, o => [o.age,o.weight,o.length,o.width]]
                };
                exports.hi2 = v => {
                    var players2 = db.players2(action.account, action.account, indexes);
                    var itr = players2.indexes.detail1.find({age:49,weight:101});

                    var is_end = itr.is_end();
                    while(!is_end) {
                        console.log(itr.data);
                        itr = itr.next();
                        is_end = itr.is_end();
                    }
                }`;
                fibos.setcodeSync(name, 0, 0, fibos.compileCode(js_code));

                ctx = fibos.contractSync(name);

                var r = ctx.hi2Sync(2, {
                    authorization: name
                });
                assert.equal(r.processed.action_traces[0].console, "{\n  \"title\": \"cto\",\n  \"age\": 49n,\n  \"weight\": 101n,\n  \"length\": 171n,\n  \"width\": 101n\n}\n{\n  \"title\": \"cfo\",\n  \"age\": 49n,\n  \"weight\": 101n,\n  \"length\": 172n,\n  \"width\": 102n\n}\n{\n  \"title\": \"cio\",\n  \"age\": 48n,\n  \"weight\": 102n,\n  \"length\": 170n,\n  \"width\": 103n\n}\n");
            });

            it('next for 192bit', () => {
                var js_code = `
                const indexes = {
                    age:[64, o=>[o.age]],
                    detail1:[128, o => [o.age,o.weight]],
                    detail2:[192, o => [o.age,o.weight,o.length]],
                    detail3:[256, o => [o.age,o.weight,o.length,o.width]]
                };
                exports.hi2 = v => {
                    var players2 = db.players2(action.account, action.account, indexes);
                    var itr = players2.indexes.detail2.find({age:49,weight:101,length:171});

                    var is_end = itr.is_end();
                    while(!is_end) {
                        console.log(itr.data);
                        itr = itr.next();
                        is_end = itr.is_end();
                    }                    
                }`;
                fibos.setcodeSync(name, 0, 0, fibos.compileCode(js_code));

                ctx = fibos.contractSync(name);

                var r = ctx.hi2Sync(3, {
                    authorization: name
                });
                assert.equal(r.processed.action_traces[0].console, "{\n  \"title\": \"cto\",\n  \"age\": 49n,\n  \"weight\": 101n,\n  \"length\": 171n,\n  \"width\": 101n\n}\n{\n  \"title\": \"cfo\",\n  \"age\": 49n,\n  \"weight\": 101n,\n  \"length\": 172n,\n  \"width\": 102n\n}\n{\n  \"title\": \"cio\",\n  \"age\": 48n,\n  \"weight\": 102n,\n  \"length\": 170n,\n  \"width\": 103n\n}\n");
            });

            it('next for 256bit', () => {
                var js_code = `
                const indexes = {
                    age:[64, o=>[o.age]],
                    detail1:[128, o => [o.age,o.weight]],
                    detail2:[192, o => [o.age,o.weight,o.length]],
                    detail3:[256, o => [o.age,o.weight,o.length,o.width]]
                };
                exports.hi2 = v => {
                    var players2 = db.players2(action.account, action.account, indexes);
                    var itr = players2.indexes.detail3.find({age:49,weight:101,length:171,width:101});

                    var is_end = itr.is_end();
                    while(!is_end) {
                        console.log(itr.data);
                        itr = itr.next();
                        is_end = itr.is_end();
                    }
                }`;
                fibos.setcodeSync(name, 0, 0, fibos.compileCode(js_code));

                ctx = fibos.contractSync(name);

                var r = ctx.hi2Sync(4, {
                    authorization: name
                });
                assert.equal(r.processed.action_traces[0].console, "{\n  \"title\": \"cto\",\n  \"age\": 49n,\n  \"weight\": 101n,\n  \"length\": 171n,\n  \"width\": 101n\n}\n{\n  \"title\": \"cfo\",\n  \"age\": 49n,\n  \"weight\": 101n,\n  \"length\": 172n,\n  \"width\": 102n\n}\n{\n  \"title\": \"cio\",\n  \"age\": 48n,\n  \"weight\": 102n,\n  \"length\": 170n,\n  \"width\": 103n\n}\n");
            });
        })

        describe('previous', () => {
            it('previous for 64bit', () => {
                var js_code = `
                const indexes = {
                    age:[64, o=>[o.age]],
                    detail1:[128, o => [o.age,o.weight]],
                    detail2:[192, o => [o.age,o.weight,o.length]],
                    detail3:[256, o => [o.age,o.weight,o.length,o.width]]
                };
                exports.hi2 = v => {
                    var players2 = db.players2(action.account, action.account, indexes);
                    var itr = players2.indexes.age.find({age:49});

                    var is_end = itr.is_end();
                    let i = 0;
                    while(!is_end) {
                        itr = itr.next();
                        is_end = itr.is_end();
                        i++;
                    }

                    var j = 0;

                    while(j < i) {
                        itr = itr.previous();
                        console.log(itr.data);
                        is_end = itr.is_end();
                        j++;
                    }
                }`;
                fibos.setcodeSync(name, 0, 0, fibos.compileCode(js_code));

                ctx = fibos.contractSync(name);

                var r = ctx.hi2Sync(100, {
                    authorization: name
                });
                assert.equal(r.processed.action_traces[0].console, "{\n  \"title\": \"cso\",\n  \"age\": 50n,\n  \"weight\": 100n,\n  \"length\": 170n,\n  \"width\": 100n\n}\n{\n  \"title\": \"cfo\",\n  \"age\": 49n,\n  \"weight\": 101n,\n  \"length\": 172n,\n  \"width\": 102n\n}\n{\n  \"title\": \"cto\",\n  \"age\": 49n,\n  \"weight\": 101n,\n  \"length\": 171n,\n  \"width\": 101n\n}\n");
            });

            it('previous for 128bit', () => {
                var js_code = `
                const indexes = {
                    age:[64, o=>[o.age]],
                    detail1:[128, o => [o.age,o.weight]],
                    detail2:[192, o => [o.age,o.weight,o.length]],
                    detail3:[256, o => [o.age,o.weight,o.length,o.width]]
                };
                exports.hi2 = v => {
                    var players2 = db.players2(action.account, action.account, indexes);
                    var itr = players2.indexes.detail1.find({age:49,weight:101});

                    var is_end = itr.is_end();
                    let i = 0;
                    while(!is_end) {
                        itr = itr.next();
                        is_end = itr.is_end();
                        i++;
                    }

                    var j = 0;
                    while(j < i) {
                        itr = itr.previous();
                        console.log(itr.data);
                        is_end = itr.is_end();
                        j++;
                    }
                }`;
                fibos.setcodeSync(name, 0, 0, fibos.compileCode(js_code));

                ctx = fibos.contractSync(name);

                var r = ctx.hi2Sync(2, {
                    authorization: name
                });
                assert.equal(r.processed.action_traces[0].console, "{\n  \"title\": \"cio\",\n  \"age\": 48n,\n  \"weight\": 102n,\n  \"length\": 170n,\n  \"width\": 103n\n}\n{\n  \"title\": \"cfo\",\n  \"age\": 49n,\n  \"weight\": 101n,\n  \"length\": 172n,\n  \"width\": 102n\n}\n{\n  \"title\": \"cto\",\n  \"age\": 49n,\n  \"weight\": 101n,\n  \"length\": 171n,\n  \"width\": 101n\n}\n");
            });

            it('previous for 192bit', () => {
                var js_code = `
                const indexes = {
                    age:[64, o=>[o.age]],
                    detail1:[128, o => [o.age,o.weight]],
                    detail2:[192, o => [o.age,o.weight,o.length]],
                    detail3:[256, o => [o.age,o.weight,o.length,o.width]]
                };
                exports.hi2 = v => {
                    var players2 = db.players2(action.account, action.account, indexes);
                    var itr = players2.indexes.detail2.find({age:49,weight:101,length:171});

                    var is_end = itr.is_end();
                    let i = 0;
                    while(!is_end) {
                        itr = itr.next();
                        is_end = itr.is_end();
                        i++;
                    }

                    var j = 0;
                    while(j < i) {
                        itr = itr.previous();
                        console.log(itr.data);
                        is_end = itr.is_end();
                        j++;
                    }
                }`;
                fibos.setcodeSync(name, 0, 0, fibos.compileCode(js_code));

                ctx = fibos.contractSync(name);

                var r = ctx.hi2Sync(213, {
                    authorization: name
                });
                assert.equal(r.processed.action_traces[0].console, "{\n  \"title\": \"cio\",\n  \"age\": 48n,\n  \"weight\": 102n,\n  \"length\": 170n,\n  \"width\": 103n\n}\n{\n  \"title\": \"cfo\",\n  \"age\": 49n,\n  \"weight\": 101n,\n  \"length\": 172n,\n  \"width\": 102n\n}\n{\n  \"title\": \"cto\",\n  \"age\": 49n,\n  \"weight\": 101n,\n  \"length\": 171n,\n  \"width\": 101n\n}\n");
            });

            it('previous for 256bit', () => {
                var js_code = `
                const indexes = {
                    age:[64, o=>[o.age]],
                    detail1:[128, o => [o.age,o.weight]],
                    detail2:[192, o => [o.age,o.weight,o.length]],
                    detail3:[256, o => [o.age,o.weight,o.length,o.width]]
                };
                exports.hi2 = v => {
                    var players2 = db.players2(action.account, action.account, indexes);
                    var itr = players2.indexes.detail3.find({age:49,weight:101,length:171,width:101});

                    var is_end = itr.is_end();
                    let i = 0;
                    while(!is_end) {
                        itr = itr.next();
                        is_end = itr.is_end();
                        i++;
                    }

                    var j = 0;
                    while(j < i) {
                        itr = itr.previous();
                        console.log(itr.data);
                        is_end = itr.is_end();
                        j++;
                    }
                }`;
                fibos.setcodeSync(name, 0, 0, fibos.compileCode(js_code));

                ctx = fibos.contractSync(name);

                var r = ctx.hi2Sync(432, {
                    authorization: name
                });
                assert.equal(r.processed.action_traces[0].console, "{\n  \"title\": \"cio\",\n  \"age\": 48n,\n  \"weight\": 102n,\n  \"length\": 170n,\n  \"width\": 103n\n}\n{\n  \"title\": \"cfo\",\n  \"age\": 49n,\n  \"weight\": 101n,\n  \"length\": 172n,\n  \"width\": 102n\n}\n{\n  \"title\": \"cto\",\n  \"age\": 49n,\n  \"weight\": 101n,\n  \"length\": 171n,\n  \"width\": 101n\n}\n");
            });
        })

        describe('lowerbound', () => {
            it('lowerbound for 64bit', () => {
                var js_code = `
                const indexes = {
                    age:[64, o=>[o.age]],
                    detail1:[128, o => [o.age,o.weight]],
                    detail2:[192, o => [o.age,o.weight,o.length]],
                    detail3:[256, o => [o.age,o.weight,o.length,o.width]]
                };
                exports.hi2 = v => {
                    var players2 = db.players2(action.account, action.account, indexes);

                    var itr = players2.indexes.age.lowerbound({age:49});
                    console.log(itr.data);
                    console.log(itr.previous().data);
                    console.log(itr.next().data);

                    itr = players2.indexes.age.lowerbound({age:45});
                    console.log(itr.data);
                    console.log(itr.previous().data);
                    console.log(itr.next().data);

                    itr = players2.indexes.age.lowerbound({age:55});
                    console.log(itr.data);
                    console.log(itr.previous().data);
                    console.log(itr.next().data);
                }`;
                fibos.setcodeSync(name, 0, 0, fibos.compileCode(js_code));

                ctx = fibos.contractSync(name);

                var r = ctx.hi2Sync(100, {
                    authorization: name
                });
                assert.equal(r.processed.action_traces[0].console, "{\n  \"title\": \"cto\",\n  \"age\": 49n,\n  \"weight\": 101n,\n  \"length\": 171n,\n  \"width\": 101n\n}\n{\n  \"title\": \"cio\",\n  \"age\": 48n,\n  \"weight\": 102n,\n  \"length\": 170n,\n  \"width\": 103n\n}\n{\n  \"title\": \"cfo\",\n  \"age\": 49n,\n  \"weight\": 101n,\n  \"length\": 172n,\n  \"width\": 102n\n}\n{\n  \"title\": \"ceo\",\n  \"age\": 48n,\n  \"weight\": 100n,\n  \"length\": 170n,\n  \"width\": 100n\n}\nundefined\n{\n  \"title\": \"coo\",\n  \"age\": 48n,\n  \"weight\": 101n,\n  \"length\": 171n,\n  \"width\": 101n\n}\nundefined\n{\n  \"title\": \"cso\",\n  \"age\": 50n,\n  \"weight\": 100n,\n  \"length\": 170n,\n  \"width\": 100n\n}\nundefined\n");

            });

            it('lowerbound for 128bit', () => {
                var js_code = `
                const indexes = {
                    age:[64, o=>[o.age]],
                    detail1:[128, o => [o.age,o.weight]],
                    detail2:[192, o => [o.age,o.weight,o.length]],
                    detail3:[256, o => [o.age,o.weight,o.length,o.width]]
                };
                exports.hi2 = v => {
                    var players2 = db.players2(action.account, action.account, indexes);

                    var itr = players2.indexes.detail1.lowerbound({age:49,weight:101});
                    console.log(itr.data);
                    console.log(itr.previous().data);
                    console.log(itr.next().data);

                    itr = players2.indexes.detail1.lowerbound({age:49,weight:95});
                    console.log(itr.data);
                    console.log(itr.previous().data);
                    console.log(itr.next().data);

                    itr = players2.indexes.detail1.lowerbound({age:49,weight:104});
                    console.log(itr.data);
                    console.log(itr.previous().data);
                    console.log(itr.next().data);
                }`;
                fibos.setcodeSync(name, 0, 0, fibos.compileCode(js_code));

                ctx = fibos.contractSync(name);

                var r = ctx.hi2Sync(2, {
                    authorization: name
                });
                assert.equal(r.processed.action_traces[0].console, "{\n  \"title\": \"cto\",\n  \"age\": 49n,\n  \"weight\": 101n,\n  \"length\": 171n,\n  \"width\": 101n\n}\n{\n  \"title\": \"coo\",\n  \"age\": 48n,\n  \"weight\": 101n,\n  \"length\": 171n,\n  \"width\": 101n\n}\n{\n  \"title\": \"cfo\",\n  \"age\": 49n,\n  \"weight\": 101n,\n  \"length\": 172n,\n  \"width\": 102n\n}\n{\n  \"title\": \"ceo\",\n  \"age\": 48n,\n  \"weight\": 100n,\n  \"length\": 170n,\n  \"width\": 100n\n}\nundefined\n{\n  \"title\": \"cso\",\n  \"age\": 50n,\n  \"weight\": 100n,\n  \"length\": 170n,\n  \"width\": 100n\n}\nundefined\n{\n  \"title\": \"cio\",\n  \"age\": 48n,\n  \"weight\": 102n,\n  \"length\": 170n,\n  \"width\": 103n\n}\nundefined\n");
            });

            it('lowerbound for 192bit', () => {
                var js_code = `
                const indexes = {
                    age:[64, o=>[o.age]],
                    detail1:[128, o => [o.age,o.weight]],
                    detail2:[192, o => [o.age,o.weight,o.length]],
                    detail3:[256, o => [o.age,o.weight,o.length,o.width]]
                };
                exports.hi2 = v => {
                    var players2 = db.players2(action.account, action.account, indexes);

                    var itr = players2.indexes.detail2.lowerbound({age:49,weight:101,length:171});
                    console.log(itr.data);
                    console.log(itr.previous().data);
                    console.log(itr.next().data);

                    itr = players2.indexes.detail2.lowerbound({age:49,weight:95,length:190});
                    console.log(itr.data);
                    console.log(itr.previous().data);
                    console.log(itr.next().data);

                    itr = players2.indexes.detail2.lowerbound({age:49,weight:105,length:190});
                    console.log(itr.data);
                    console.log(itr.previous().data);
                    console.log(itr.next().data);
                }`;
                fibos.setcodeSync(name, 0, 0, fibos.compileCode(js_code));

                ctx = fibos.contractSync(name);

                var r = ctx.hi2Sync(3, {
                    authorization: name
                });
                assert.equal(r.processed.action_traces[0].console, "{\n  \"title\": \"cto\",\n  \"age\": 49n,\n  \"weight\": 101n,\n  \"length\": 171n,\n  \"width\": 101n\n}\n{\n  \"title\": \"coo\",\n  \"age\": 48n,\n  \"weight\": 101n,\n  \"length\": 171n,\n  \"width\": 101n\n}\n{\n  \"title\": \"cfo\",\n  \"age\": 49n,\n  \"weight\": 101n,\n  \"length\": 172n,\n  \"width\": 102n\n}\n{\n  \"title\": \"ceo\",\n  \"age\": 48n,\n  \"weight\": 100n,\n  \"length\": 170n,\n  \"width\": 100n\n}\nundefined\n{\n  \"title\": \"cso\",\n  \"age\": 50n,\n  \"weight\": 100n,\n  \"length\": 170n,\n  \"width\": 100n\n}\nundefined\n{\n  \"title\": \"cio\",\n  \"age\": 48n,\n  \"weight\": 102n,\n  \"length\": 170n,\n  \"width\": 103n\n}\nundefined\n");
            });

            it('lowerbound for 256bit', () => {
                var js_code = `
                const indexes = {
                    age:[64, o=>[o.age]],
                    detail1:[128, o => [o.age,o.weight]],
                    detail2:[192, o => [o.age,o.weight,o.length]],
                    detail3:[256, o => [o.age,o.weight,o.length,o.width]]
                };
                exports.hi2 = v => {
                    var players2 = db.players2(action.account, action.account, indexes);

                    var itr = players2.indexes.detail3.lowerbound({age:49,weight:101,length:171,width:101});
                    console.log(itr.data);
                    console.log(itr.previous().data);
                    console.log(itr.next().data);

                    itr = players2.indexes.detail3.lowerbound({age:49,weight:95,length:165,width:101});
                    console.log(itr.data);
                    console.log(itr.previous().data);
                    console.log(itr.next().data);

                    itr = players2.indexes.detail3.lowerbound({age:49,weight:105,length:190,width:101});
                    console.log(itr.data);
                    console.log(itr.previous().data);
                    console.log(itr.next().data);
                }`;
                fibos.setcodeSync(name, 0, 0, fibos.compileCode(js_code));

                ctx = fibos.contractSync(name);

                var r = ctx.hi2Sync(4, {
                    authorization: name
                });
                assert.equal(r.processed.action_traces[0].console, "{\n  \"title\": \"cto\",\n  \"age\": 49n,\n  \"weight\": 101n,\n  \"length\": 171n,\n  \"width\": 101n\n}\n{\n  \"title\": \"coo\",\n  \"age\": 48n,\n  \"weight\": 101n,\n  \"length\": 171n,\n  \"width\": 101n\n}\n{\n  \"title\": \"cfo\",\n  \"age\": 49n,\n  \"weight\": 101n,\n  \"length\": 172n,\n  \"width\": 102n\n}\n{\n  \"title\": \"ceo\",\n  \"age\": 48n,\n  \"weight\": 100n,\n  \"length\": 170n,\n  \"width\": 100n\n}\nundefined\n{\n  \"title\": \"cso\",\n  \"age\": 50n,\n  \"weight\": 100n,\n  \"length\": 170n,\n  \"width\": 100n\n}\nundefined\n{\n  \"title\": \"cio\",\n  \"age\": 48n,\n  \"weight\": 102n,\n  \"length\": 170n,\n  \"width\": 103n\n}\nundefined\n");
            });
        });

        describe('upperbound', () => {
            it('upperbound for 64bit', () => {
                var js_code = `
                const indexes = {
                    age:[64, o=>[o.age]],
                    detail1:[128, o => [o.age,o.weight]],
                    detail2:[192, o => [o.age,o.weight,o.length]],
                    detail3:[256, o => [o.age,o.weight,o.length,o.width]]
                };
                exports.hi2 = v => {
                    var players2 = db.players2(action.account, action.account, indexes);

                    var itr = players2.indexes.age.upperbound({age:49});
                    console.log(itr.data);
                    console.log(itr.previous().data);
                    console.log(itr.next().data);

                    itr = players2.indexes.age.upperbound({age:45});
                    console.log(itr.data);
                    console.log(itr.previous().data);
                    console.log(itr.next().data);

                    itr = players2.indexes.age.upperbound({age:55});
                    console.log(itr.data);
                }`;
                fibos.setcodeSync(name, 0, 0, fibos.compileCode(js_code));

                ctx = fibos.contractSync(name);

                var r = ctx.hi2Sync(100, {
                    authorization: name
                });
                assert.equal(r.processed.action_traces[0].console, "{\n  \"title\": \"cso\",\n  \"age\": 50n,\n  \"weight\": 100n,\n  \"length\": 170n,\n  \"width\": 100n\n}\n{\n  \"title\": \"cfo\",\n  \"age\": 49n,\n  \"weight\": 101n,\n  \"length\": 172n,\n  \"width\": 102n\n}\nundefined\n{\n  \"title\": \"ceo\",\n  \"age\": 48n,\n  \"weight\": 100n,\n  \"length\": 170n,\n  \"width\": 100n\n}\nundefined\n{\n  \"title\": \"coo\",\n  \"age\": 48n,\n  \"weight\": 101n,\n  \"length\": 171n,\n  \"width\": 101n\n}\nundefined\n");

            });

            it('upperbound for 128bit', () => {
                var js_code = `
                const indexes = {
                    age:[64, o=>[o.age]],
                    detail1:[128, o => [o.age,o.weight]],
                    detail2:[192, o => [o.age,o.weight,o.length]],
                    detail3:[256, o => [o.age,o.weight,o.length,o.width]]
                };
                exports.hi2 = v => {
                    var players2 = db.players2(action.account, action.account, indexes);

                    var itr = players2.indexes.detail1.upperbound({age:49,weight:101});
                    console.log(itr.data);
                    console.log(itr.previous().data);
                    console.log(itr.next().data);

                    itr = players2.indexes.detail1.upperbound({age:49,weight:95});
                    console.log(itr.data);
                    console.log(itr.previous().data);
                    console.log(itr.next().data);

                    itr = players2.indexes.detail1.upperbound({age:49,weight:104});
                    console.log(itr.data);
                }`;
                fibos.setcodeSync(name, 0, 0, fibos.compileCode(js_code));

                ctx = fibos.contractSync(name);

                var r = ctx.hi2Sync(2, {
                    authorization: name
                });
                assert.equal(r.processed.action_traces[0].console, "{\n  \"title\": \"cio\",\n  \"age\": 48n,\n  \"weight\": 102n,\n  \"length\": 170n,\n  \"width\": 103n\n}\n{\n  \"title\": \"cfo\",\n  \"age\": 49n,\n  \"weight\": 101n,\n  \"length\": 172n,\n  \"width\": 102n\n}\nundefined\n{\n  \"title\": \"ceo\",\n  \"age\": 48n,\n  \"weight\": 100n,\n  \"length\": 170n,\n  \"width\": 100n\n}\nundefined\n{\n  \"title\": \"cso\",\n  \"age\": 50n,\n  \"weight\": 100n,\n  \"length\": 170n,\n  \"width\": 100n\n}\nundefined\n");
            });

            it('upperbound for 192bit', () => {
                var js_code = `
                const indexes = {
                    age:[64, o=>[o.age]],
                    detail1:[128, o => [o.age,o.weight]],
                    detail2:[192, o => [o.age,o.weight,o.length]],
                    detail3:[256, o => [o.age,o.weight,o.length,o.width]]
                };
                exports.hi2 = v => {
                    var players2 = db.players2(action.account, action.account, indexes);

                    var itr = players2.indexes.detail2.upperbound({age:49,weight:101,length:171});
                    console.log(itr.data);
                    console.log(itr.previous().data);
                    console.log(itr.next().data);

                    itr = players2.indexes.detail2.upperbound({age:49,weight:95,length:165});
                    console.log(itr.data);
                    console.log(itr.previous().data);
                    console.log(itr.next().data);

                    itr = players2.indexes.detail2.upperbound({age:49,weight:105,length:190});
                    console.log(itr.data);
                }`;
                fibos.setcodeSync(name, 0, 0, fibos.compileCode(js_code));

                ctx = fibos.contractSync(name);

                var r = ctx.hi2Sync(3, {
                    authorization: name
                });
                assert.equal(r.processed.action_traces[0].console, "{\n  \"title\": \"cfo\",\n  \"age\": 49n,\n  \"weight\": 101n,\n  \"length\": 172n,\n  \"width\": 102n\n}\n{\n  \"title\": \"cto\",\n  \"age\": 49n,\n  \"weight\": 101n,\n  \"length\": 171n,\n  \"width\": 101n\n}\n{\n  \"title\": \"cio\",\n  \"age\": 48n,\n  \"weight\": 102n,\n  \"length\": 170n,\n  \"width\": 103n\n}\n{\n  \"title\": \"ceo\",\n  \"age\": 48n,\n  \"weight\": 100n,\n  \"length\": 170n,\n  \"width\": 100n\n}\nundefined\n{\n  \"title\": \"cso\",\n  \"age\": 50n,\n  \"weight\": 100n,\n  \"length\": 170n,\n  \"width\": 100n\n}\nundefined\n");
            });

            it('upperbound for 256bit', () => {
                var js_code = `
                const indexes = {
                    age:[64, o=>[o.age]],
                    detail1:[128, o => [o.age,o.weight]],
                    detail2:[192, o => [o.age,o.weight,o.length]],
                    detail3:[256, o => [o.age,o.weight,o.length,o.width]]
                };
                exports.hi2 = v => {
                    var players2 = db.players2(action.account, action.account, indexes);

                    var itr = players2.indexes.detail3.upperbound({age:49,weight:101,length:171,width:101});
                    console.log(itr.data);
                    console.log(itr.previous().data);
                    console.log(itr.next().data);

                    itr = players2.indexes.detail3.upperbound({age:49,weight:95,length:165,width:101});
                    console.log(itr.data);
                    console.log(itr.previous().data);
                    console.log(itr.next().data);

                    itr = players2.indexes.detail3.upperbound({age:49,weight:105,length:190,width:101});
                    console.log(itr.data);
                }`;
                fibos.setcodeSync(name, 0, 0, fibos.compileCode(js_code));

                ctx = fibos.contractSync(name);

                var r = ctx.hi2Sync(4, {
                    authorization: name
                });
                assert.equal(r.processed.action_traces[0].console, "{\n  \"title\": \"cfo\",\n  \"age\": 49n,\n  \"weight\": 101n,\n  \"length\": 172n,\n  \"width\": 102n\n}\n{\n  \"title\": \"cto\",\n  \"age\": 49n,\n  \"weight\": 101n,\n  \"length\": 171n,\n  \"width\": 101n\n}\n{\n  \"title\": \"cio\",\n  \"age\": 48n,\n  \"weight\": 102n,\n  \"length\": 170n,\n  \"width\": 103n\n}\n{\n  \"title\": \"ceo\",\n  \"age\": 48n,\n  \"weight\": 100n,\n  \"length\": 170n,\n  \"width\": 100n\n}\nundefined\n{\n  \"title\": \"cso\",\n  \"age\": 50n,\n  \"weight\": 100n,\n  \"length\": 170n,\n  \"width\": 100n\n}\nundefined\n");
            });
        });

        it('update', () => {
            var js_code = `
            const indexes = {
                age:[64, o=>[o.age]],
                detail1:[128, o => [o.age,o.weight]],
                detail2:[192, o => [o.age,o.weight,o.length]],
                detail3:[256, o => [o.age,o.weight,o.length,o.width]]
            };
            exports.hi2 = v => {
                var players2 = db.players2(action.account, action.account, indexes);
                var itr = players2.indexes.age.find({age:48});
                itr.data.age = 18;
                itr.update(action.account);
                var itr1 = players2.indexes.detail2.find({age:18,weight:100,length:170});
                console.log(itr1.data);
                var itr2 = players2.indexes.age.find({age:18});
                console.log(itr2.is_end()); 
            }`;
            fibos.setcodeSync(name, 0, 0, fibos.compileCode(js_code));

            ctx = fibos.contractSync(name);

            var r = ctx.hi2Sync(5, {
                authorization: name
            });

            assert.equal(r.processed.action_traces[0].console, '{\n  \"title\": \"ceo\",\n  \"age\": 18n,\n  \"weight\": 100n,\n  \"length\": 170n,\n  \"width\": 100n\n}\nfalse\n');
        });

        it('remove', () => {
            var js_code = `
            const indexes = {
                age:[64, o=>[o.age]],
                detail1:[128, o => [o.age,o.weight]],
                detail2:[192, o => [o.age,o.weight,o.length]],
                detail3:[256, o => [o.age,o.weight,o.length,o.width]]
            };
            exports.hi2 = v => {
                var players2 = db.players2(action.account, action.account, indexes);
                var itr = players2.indexes.age.find({age:18});
                itr.remove();
                var itr2 = players2.indexes.age.find({age:18});
                console.log(itr2.is_end());
                var itr3 = players2.indexes.detail3.find({age:18,weight:100,length:170,width:100});
                console.log(itr3.is_end());
            }`;
            fibos.setcodeSync(name, 0, 0, fibos.compileCode(js_code));

            ctx = fibos.contractSync(name);

            var r = ctx.hi2Sync(6, {
                authorization: name
            });

            assert.equal(r.processed.action_traces[0].console, 'true\ntrue\n');
        });
    })

    describe('primary key', () => {
        it('get primary key', () => {
            var js_code = `exports.hi = v => {
                var players = db.players3(action.account, action.account);
                var pkey = players.get_primary_key();
                console.log(pkey);
            }`;
            fibos.setcodeSync(name, 0, 0, fibos.compileCode(js_code));

            ctx = fibos.contractSync(name);
            var r = ctx.hiSync("lion1", {
                authorization: name
            });
            assert.equal(r.processed.action_traces[0].console, '0n\n');
        });

        it('emplace', () => {
            var js_code = `exports.hi = v => {
                var players = db.players3(action.account, action.account);
                var pkey1 = players.get_primary_key();
                console.log(pkey1);
                players.emplace(action.account, { title:"ceo", age: 48, weight: 100, length: 170, width: 100, id: pkey1 });

                var pkey2 = players.get_primary_key();
                console.log(pkey2);
                players.emplace(action.account, { title:"cto", age: 49, weight: 100, length: 170, width: 100, id: pkey2 });

                var pkey3 = players.get_primary_key();
                console.log(pkey3);
                players.emplace(action.account, { title:"coo", age: 50, weight: 100, length: 170, width: 100, id: pkey3 });
            }`;
            fibos.setcodeSync(name, 0, 0, fibos.compileCode(js_code));

            ctx = fibos.contractSync(name);
            var r = ctx.hiSync("lion2", {
                authorization: name
            });
            assert.equal(r.processed.action_traces[0].console, '0n\n1n\n2n\n');
        });

        it('find by primary key', () => {
            var js_code = `exports.hi1 = v => {
                var players = db.players3(action.account, action.account);
                console.log(players.find(v).data);
            }`;
            fibos.setcodeSync(name, 0, 0, fibos.compileCode(js_code));

            var r = ctx.hi1Sync(0, {
                authorization: name
            });
            assert.equal(r.processed.action_traces[0].console, "{\n  \"title\": \"ceo\",\n  \"age\": 48n,\n  \"weight\": 100n,\n  \"length\": 170n,\n  \"width\": 100n\n}\n");

            r = ctx.hi1Sync(1, {
                authorization: name
            });
            assert.equal(r.processed.action_traces[0].console, "{\n  \"title\": \"cto\",\n  \"age\": 49n,\n  \"weight\": 100n,\n  \"length\": 170n,\n  \"width\": 100n\n}\n");

            r = ctx.hi1Sync(2, {
                authorization: name
            });
            assert.equal(r.processed.action_traces[0].console, "{\n  \"title\": \"coo\",\n  \"age\": 50n,\n  \"weight\": 100n,\n  \"length\": 170n,\n  \"width\": 100n\n}\n");
        });

        it('not found', () => {
            var r = ctx.hi1Sync(3, {
                authorization: name
            });
            assert.equal(r.processed.action_traces[0].console, "undefined\n");
        });

        it('primary key for new table should start from zero', () => {
            var js_code = `exports.hi = v => {
                var players = db.players3(action.account, action.account);
                var pkey = players.get_primary_key();
                console.log(pkey);

                var players1 = db.players4(action.account, action.account);
                var pkey1 = players1.get_primary_key();
                console.log(pkey1);
            }`;
            fibos.setcodeSync(name, 0, 0, fibos.compileCode(js_code));

            ctx = fibos.contractSync(name);
            var r = ctx.hiSync("lion2", {
                authorization: name
            });
            assert.equal(r.processed.action_traces[0].console, '3n\n0n\n');
        });
    })

    describe('exceptions', () => {
        it('undefined key_names in table abi should throw', () => {
            var js_code = `exports.hi = v => {
                var players = db.players5(action.account, action.account);}`;
            fibos.setcodeSync(name, 0, 0, fibos.compileCode(js_code));

            ctx = fibos.contractSync(name);
            assert.throws(() => {
                var r = ctx.hiSync('emplace', {
                    authorization: name
                });
            });
        });

        it('undefined key_types in table abi should throw', () => {
            var js_code = `exports.hi = v => {
                var players = db.player11(action.account, action.account);}`;
            fibos.setcodeSync(name, 0, 0, fibos.compileCode(js_code));

            ctx = fibos.contractSync(name);
            assert.throws(() => {
                var r = ctx.hiSync('emplace', {
                    authorization: name
                });
            });
        });
    });

    describe('state', () => {
        it('init with undefined', () => {
            var js_code = `exports.hi = v => {
                console.log(db.state);
            }`;
            fibos.setcodeSync(name, 0, 0, fibos.compileCode(js_code));

            ctx = fibos.contractSync(name);
            var r = ctx.hiSync('state1', {
                authorization: name
            });

            assert.equal(r.processed.action_traces[0].console, "undefined\n");
        })

        it('set object', () => {
            var js_code = `exports.hi = v => {
                db.state = {b:100};
                console.log(db.state);
            }`;
            fibos.setcodeSync(name, 0, 0, fibos.compileCode(js_code));

            ctx = fibos.contractSync(name);
            var r = ctx.hiSync('state2', {
                authorization: name
            });
            assert.equal(r.processed.action_traces[0].console, "{\n  \"b\": 100\n}\n");
        })

        it('update', () => {
            var js_code = `exports.hi = v => {
                db.state.b++;
                console.log(db.state);
            }`;
            fibos.setcodeSync(name, 0, 0, fibos.compileCode(js_code));
            ctx = fibos.contractSync(name);
            var r = ctx.hiSync('state3', {
                authorization: name
            });
            assert.equal(r.processed.action_traces[0].console, "{\n  \"b\": 101\n}\n");

            ctx = fibos.contractSync(name);
            var r = ctx.hiSync('state4', {
                authorization: name
            });
            assert.equal(r.processed.action_traces[0].console, "{\n  \"b\": 102\n}\n");
        })

        it('not share with contract', () => {
            var js_code = `exports.hi = v => {
                console.log(db.state);
            }`;
            fibos.setcodeSync(name1, 0, 0, fibos.compileCode(js_code));

            ctx = fibos.contractSync(name1);
            var r = ctx.hiSync('state5', {
                authorization: name1
            });

            assert.equal(r.processed.action_traces[0].console, "undefined\n");
        })
    });

    it('begin', () => {
        var js_code = `exports.hi1 = v => {
            var players = db.players1(action.account, action.account);
            var itr = players.begin();
            console.log(itr.data, itr.is_begin(), itr.next().data);}`;
        fibos.setcodeSync(name, 0, 0, fibos.compileCode(js_code));

        ctx = fibos.contractSync(name);
        var r = ctx.hi1Sync(1, {
            authorization: name
        });
        assert.equal(r.processed.action_traces[0].console, '{\n  \"title\": \"ceo2\",\n  \"age\": 50n\n} true undefined\n');

        js_code = `exports.hi1 = v => {
            var players = db.players2(action.account, action.account);
            var itr = players.begin();
            console.log(itr.data, itr.is_begin(), itr.next().data);}`;
        fibos.setcodeSync(name, 0, 0, fibos.compileCode(js_code));

        r = ctx.hi1Sync(11, {
            authorization: name
        });
        assert.equal(r.processed.action_traces[0].console, '{\n  \"title\": \"cto\",\n  \"age\": 49n,\n  \"weight\": 101n,\n  \"length\": 171n,\n  \"width\": 101n\n} true {\n  \"title\": \"coo\",\n  \"age\": 48n,\n  \"weight\": 101n,\n  \"length\": 171n,\n  \"width\": 101n\n}\n');

        js_code = `exports.hi1 = v => {
            var players = db.players3(action.account, action.account);
            var itr = players.begin();
            console.log(itr.data, itr.is_begin(), itr.next().data);}`;
        fibos.setcodeSync(name, 0, 0, fibos.compileCode(js_code));
        r = ctx.hi1Sync(1, {
            authorization: name
        });
        assert.equal(r.processed.action_traces[0].console, '{\n  \"title\": \"ceo\",\n  \"age\": 48n,\n  \"weight\": 100n,\n  \"length\": 170n,\n  \"width\": 100n\n} true {\n  \"title\": \"cto\",\n  \"age\": 49n,\n  \"weight\": 100n,\n  \"length\": 170n,\n  \"width\": 100n\n}\n');

        js_code = `exports.hi1 = v => {
            var players = db.players4(action.account, action.account);
            var itr = players.begin();
            console.log(itr.data, itr.is_begin(), itr.next().data);}`;
        fibos.setcodeSync(name, 0, 0, fibos.compileCode(js_code));
        r = ctx.hi1Sync(15, {
            authorization: name
        });
        assert.equal(r.processed.action_traces[0].console, 'undefined false undefined\n');
    });

    it('end', () => {
        var js_code = `exports.hi1 = v => {
            var players = db.players1(action.account, action.account);
            var itr = players.end();
            console.log(itr.data, itr.is_end(), itr.previous().data);}`;
        fibos.setcodeSync(name, 0, 0, fibos.compileCode(js_code));

        ctx = fibos.contractSync(name);
        var r = ctx.hi1Sync(12, {
            authorization: name
        });
        assert.equal(r.processed.action_traces[0].console, 'undefined true {\n  \"title\": \"ceo2\",\n  \"age\": 50n\n}\n');

        js_code = `exports.hi1 = v => {
            var players = db.players2(action.account, action.account);
            var itr = players.end();
            console.log(itr.data, itr.is_end(), itr.previous().data);}`;
        fibos.setcodeSync(name, 0, 0, fibos.compileCode(js_code));

        r = ctx.hi1Sync(11, {
            authorization: name
        });
        assert.equal(r.processed.action_traces[0].console, 'undefined true {\n  \"title\": \"cso\",\n  \"age\": 50n,\n  \"weight\": 100n,\n  \"length\": 170n,\n  \"width\": 100n\n}\n');

        js_code = `exports.hi1 = v => {
            var players = db.players3(action.account, action.account);
            var itr = players.end();
            console.log(itr.data, itr.is_end(), itr.previous().data);}`;
        fibos.setcodeSync(name, 0, 0, fibos.compileCode(js_code));
        r = ctx.hi1Sync(1, {
            authorization: name
        });
        assert.equal(r.processed.action_traces[0].console, 'undefined true {\n  \"title\": \"coo\",\n  \"age\": 50n,\n  \"weight\": 100n,\n  \"length\": 170n,\n  \"width\": 100n\n}\n');

        js_code = `exports.hi1 = v => {
            var players = db.players4(action.account, action.account);
            var itr = players.end();
            console.log(itr.data, itr.is_end());}`;
        fibos.setcodeSync(name, 0, 0, fibos.compileCode(js_code));
        r = ctx.hi1Sync(11, {
            authorization: name
        });
        assert.equal(r.processed.action_traces[0].console, 'undefined true\n');
    });

    it('fix db get', () => {
        var js_code_1 = `
            const indexes = {
                age:[64, o=>[o.age]],
                detail1:[128, o => [o.age,o.weight]],
                detail2:[192, o => [o.age,o.weight,o.length]],
                detail3:[256, o => [o.age,o.weight,o.length,o.width]]
            };

            exports.hi1 = v => {
                var players12 = db.players12(action.account, action.account, indexes);
                players12.emplace(action.account, {title:"coo",age:48,weight:101,length:171,width:101,id:4});
                players12.emplace(action.account, {title:"cfo",age:49,weight:101,length:172,width:102,id:5});
                players12.emplace(action.account, {title:"cio",age:48,weight:102,length:170,width:103,id:6});
                players12.emplace(action.account, {title:"cso",age:50,weight:100,length:170,width:100,id:7});
            };

            exports.hi2 = v => {
                var players12 = db.players12(action.account, action.account, indexes);

                var itr = players12.indexes.age.lowerbound({age:49});
                while(itr && !itr.is_end()) {
                    var temp = itr.next();
                    itr.remove();
                    itr = temp;
                }

                var itr_result = players12.indexes.age.lowerbound({age:0});
                while(itr_result && !itr_result.is_end()) {
                    console.log(itr_result.data);
                    itr_result = itr_result.next();
                }
            }
        `;

        var js_code_2 = `
            const indexes = {
                age:[64, o=>[o.age]],
                detail1:[128, o => [o.age,o.weight]],
                detail2:[192, o => [o.age,o.weight,o.length]],
                detail3:[256, o => [o.age,o.weight,o.length,o.width]]
            };

            exports.hi1 = v => {
                var players12 = db.players12(action.account, action.account, indexes);
                players12.emplace(action.account, {title:"coo",age:48,weight:101,length:171,width:101,id:players12.get_primary_key()});
                players12.emplace(action.account, {title:"cfo",age:49,weight:101,length:172,width:102,id:players12.get_primary_key()});
                players12.emplace(action.account, {title:"cio",age:48,weight:102,length:170,width:103,id:players12.get_primary_key()});
                players12.emplace(action.account, {title:"cso",age:50,weight:100,length:170,width:100,id:players12.get_primary_key()});
            };

            exports.hi2 = v => {
                var players12 = db.players12(action.account, action.account, indexes);

                var itr = players12.lowerbound(0);
                while(itr && !itr.is_end()) {
                    var temp = itr.next();
                    itr.remove();
                    itr = temp;
                }

                var itr_result = players12.lowerbound(0);
                while(itr_result && !itr_result.is_end()) {
                    console.log(itr_result.data);
                    itr_result = itr_result.next();
                }
            }
        `;

        fibos.setcodeSync(name, 0, 0, fibos.compileCode(js_code_1));

        let ctx = fibos.contractSync(name);
        let r = ctx.hi1Sync(123, {
            authorization: name
        });
        r = ctx.hi2Sync(123, {
            authorization: name
        });

        fibos.setcodeSync(name, 0, 0, fibos.compileCode(js_code_2));
        ctx = fibos.contractSync(name);

        r = ctx.hi1Sync(1234, {
            authorization: name
        });
        r = ctx.hi2Sync(234, {
            authorization: name
        });
    });

    it('fix primary key', () => {
        var js_code = `
            const indexes = {
                age:[64, o=>[o.age]],
                detail1:[128, o => [o.age,o.weight]],
                detail2:[192, o => [o.age,o.weight,o.length]],
                detail3:[256, o => [o.age,o.weight,o.length,o.width]]
            };

            exports.hi1 = v => {
                var players2 = db.players2(action.account, action.account, indexes);
                players2.emplace(action.account, {title:"coo",age:48,weight:101,length:171,width:101,id:10});

                var players3 = db.players3(action.account, action.account, indexes);
                players3.emplace(action.account, {title:"coo",age:48,weight:101,length:171,width:101,id:10});
            };
        `;

        fibos.setcodeSync(name, 0, 0, fibos.compileCode(js_code));

        let ctx = fibos.contractSync(name);
        let r = ctx.hi1Sync(123, {
            authorization: name
        });

    });

    describe('different struct of indexes', () => {
        it('emplace by primary key', () => {
            console.log(fibos.getTableRowsSync(true, name, name, 'players13'))
            var js_code = `
                const indexes = {
                    byname:[64, o=>[o.name]]
                };

                exports.hi = v => {
                    var players_idx = db.players13(action.account, action.account, indexes);
                    players_idx.emplace(action.account, {id:players_idx.get_primary_key(),name: v});

                    var players = db.players13(action.account, action.account);
                    players.lowerbound(0).remove()

                    players.emplace(action.account, {id:players.get_primary_key(),name: v});
                };
            `;

            fibos.setcodeSync(name, 0, 0, fibos.compileCode(js_code));

            let ctx = fibos.contractSync(name);
            let r = ctx.hiSync("name", {
                authorization: name
            });

            r = fibos.getTableRowsSync(true, name, name, 'players13');
            assert.equal(r.rows[0].id, 0);
        });

        it('find by different index struct', () => {
            var js_code = `
                const indexes = {
                    byname:[64, o=>[o.name]]
                };

                const indexes_1 = {
                    byname:[64, o=>[o.name]],
                    byname1:[128, o=>[o.name, o.id]],
                };

                exports.hi = v => {
                    var players = db.players13(action.account, action.account, indexes);
                    players.emplace(action.account, {id:players.get_primary_key(),name: "name1"});

                    var players = db.players13(action.account, action.account, indexes_1);
                    var itr = players.indexes.byname.find({name: "name"});
                    console.log(itr);

                    itr.remove();

                    var players1 = db.players13(action.account, action.account, indexes_1);
                    var itr1 = players1.indexes.byname1.find({name: "name"});
                    console.log(itr1);
                };
            `;
            fibos.setcodeSync(name, 0, 0, fibos.compileCode(js_code));

            let ctx = fibos.contractSync(name);
            assert.throws(() => {
                let r = ctx.hiSync("name1", {
                    authorization: name
                });
            });

        });
    });

});

require.main === module && test.run(console.DEBUG);