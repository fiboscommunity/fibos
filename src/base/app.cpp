#define public_key_legacy_prefix public_key_legacy_prefix_eos
#include <fc/crypto/public_key.hpp>
#undef public_key_legacy_prefix

#include "config.hpp"

#include <unicode/include/unicode/uloc.h>

#include "fibjs.h"
#include "object.h"
#include "Fiber.h"
#include "console.h"
#include "SandBox.h"
#include "EventEmitter.h"
#include "../js_contract/include/js_module.h"

#include <eosio/chain/application.hpp>
#include <eosio/chain_api_plugin/chain_api_plugin.hpp>
#include <eosio/chain_plugin/chain_plugin.hpp>
#include <eosio/db_size_api_plugin/db_size_api_plugin.hpp>
#include <eosio/http_client_plugin/http_client_plugin.hpp>
#include <eosio/http_plugin/http_plugin.hpp>
#include <eosio/net_api_plugin/net_api_plugin.hpp>
#include <eosio/net_plugin/net_plugin.hpp>
#include <eosio/producer_api_plugin/producer_api_plugin.hpp>
#include <eosio/producer_plugin/producer_plugin.hpp>
#include <eosio/resource_monitor_plugin/resource_monitor_plugin.hpp>
#include <eosio/trace_api/trace_api_plugin.hpp>
#include <eosio/version/version.hpp>
#include <chainbase/pinnable_mapped_file.hpp>
#include <chainbase/environment.hpp>
#include <eosio/wallet_api_plugin/wallet_api_plugin.hpp>
#include <eosio/wallet_plugin/wallet_plugin.hpp>

#include <fc/log/logger_config.hpp>
#include <fc/log/appender.hpp>
#include <fc/exception/exception.hpp>

#include "ifs/util.h"

#include "ifs/chain.h"
#include "ifs/action.h"
#include "ifs/bc_console.h"
#include "ifs/bc_crypto.h"
#include "ifs/assert.h"
#include "ifs/trans.h"
#include "../js_contract/include/Table.h"
#include "emitter_plugin.h"
#include "../cross/include/cross_plugin.h"

#include "gitinfo.h"

// only for config.hpp
using namespace std;
#include <nodeos/config.hpp>

using namespace appbase;
using namespace eosio;
using namespace v8;

namespace fc {
namespace crypto {
    namespace config {
        // public key prefix
        const char* public_key_legacy_prefix = "PUB";
    };
}
}

namespace eosio {
namespace chain {
    static const char* base_root_key = "6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV";
    std::string chain_root_key = "PUB6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV";
}
} // namespace eosio::chain

namespace appbase {
const char* appbase_version_string = "v" GIT_INFO;
} // namespace appbase

namespace fibjs {

extern obj_ptr<NObject> g_info;
extern exlib::string appname;

DECLARE_MODULE(chain);

enum return_codes {
    OTHER_FAIL = -2,
    INITIALIZE_FAIL = -1,
    SUCCESS = 0,
    BAD_ALLOC = 1,
    DATABASE_DIRTY = 2,
    FIXED_REVERSIBLE = 3,
    EXTRACTED_GENESIS = 4
};

static std::string s_chainid = "6aa7bd33b6b45192465afa3553dedb531acaaff8928cf64b70bd4c5e49b7ec6a";
static std::vector<exlib::string> s_args = { "chain" };

static bool s_state_started = false;
static bool s_state_stopped = false;

static exlib::OSSemaphore s_sem;

void init_chain()
{
    appname = "FIBOS OS v5.0.3.0. Based on fibjs";

    g_info->remove("vender");
    g_info->add("chain", appbase_version_string);
    g_info->add("eos_version_full", (const char*)eosio::version::version_full().c_str());

    uint32_t short_hash = 0;
    fc::from_hex(eosio::version::version_hash(), (char*)&short_hash, sizeof(short_hash));

    app().set_version(htonl(short_hash));
    app().set_version_string(eosio::version::version_client());
    app().set_full_version_string(eosio::version::version_full());

    producer_plugin::set_test_mode(true);

    app()._register_plugin<chain_api_plugin>();
    app()._register_plugin<chain_plugin>();
    app()._register_plugin<db_size_api_plugin>();
    app()._register_plugin<http_client_plugin>();
    app()._register_plugin<http_plugin>();
    app()._register_plugin<net_api_plugin>();
    app()._register_plugin<net_plugin>();
    app()._register_plugin<producer_api_plugin>();
    app()._register_plugin<producer_plugin>();

    app()._register_plugin<wallet_api_plugin>();
    app()._register_plugin<wallet_plugin>();
    app()._register_plugin<resource_monitor_plugin>();
    app()._register_plugin<trace_api_plugin>();
    app()._register_plugin<emitter_plugin>();

    app()._register_plugin<cross_plugin>();

    auto root = fc::app_path();
    app().set_default_data_dir(root / "chain/data");
    app().set_default_config_dir(root / "chain/config");
}

result_t addLogger(logger* lgr);

void init_console()
{
    class os_logger : public std_logger {
    public:
        virtual void putLog(int32_t priority, exlib::string& msg)
        {
            if (Isolate::safe_current() == js_module_interface::g_isolate && g_now_context)
                g_now_context->console_append((msg + "\n").c_str());
            else
                std_logger::putLog(priority, msg);
        }
    };

    static os_logger s_logger;
    addLogger(&s_logger);
}

static void init_isolate()
{
    ilog("initializing JavaScript Isolate...");

    Isolate* isolate = js_module_interface::g_isolate;

    Runtime rt(isolate);
    v8::Locker locker(isolate->m_isolate);
    v8::Isolate::Scope isolate_scope(isolate->m_isolate);

    v8::HandleScope handle_scope(isolate->m_isolate);
    v8::Context::Scope context_scope(isolate->m_context.Get(isolate->m_isolate));

    JSFiber::EnterJsScope s;
    init_console();

    isolate->m_topSandbox = new SandBox(false);
    isolate->m_topSandbox->initGlobal(v8::Object::New(isolate->m_isolate));
    {
        ilog("initializing global module...");
        SandBox::Scope _scope(isolate->m_topSandbox);
        v8::HandleScope handle_scope(isolate->m_isolate);
        v8::Local<v8::Context> context = isolate->context();
        v8::Local<v8::Object> global = context->Global();

        ilog("Disabling JavaScript global objects...");

        v8::Local<v8::Array> properties;
        global->GetPropertyNames(context).ToLocal(&properties);
        int32_t len = properties->Length();

        for (int32_t i = 0; i < len; i++) {
            JSValue v = properties->Get(context, i);
            global->Delete(context, v.As<v8::String>()).IsJust();
        }

        global->Delete(context, isolate->NewString("WebAssembly")).IsJust();
        global->Delete(context, isolate->NewString("Intl")).IsJust();

        v8::Local<v8::Object> math = v8::Local<v8::Object>::Cast(JSValue(global->Get(context, isolate->NewString("Math"))));
        math->Delete(context, isolate->NewString("random")).IsJust();

        ilog("Disabling JavaScript locale functions...");

        v8::Local<v8::String> prototype = isolate->NewString("prototype");
        v8::Local<v8::String> toLocaleString = isolate->NewString("toLocaleString");
        v8::Local<v8::Value> undefined = v8::Undefined(isolate->m_isolate);

        static const char* s_localeobjects[] = {
            "Object", "Number", "Date", "BigInt", "Array",
            "Int8Array", "Uint8Array", "Uint8ClampedArray",
            "Int16Array", "Uint16Array", "Int32Array", "Uint32Array",
            "Float32Array", "Float64Array", "BigInt64Array", "BigUint64Array"
        };

        for (int32_t i = 0; i < ARRAYSIZE(s_localeobjects); i++) {
            v8::Local<v8::Object> object = v8::Local<v8::Object>::Cast(JSValue(global->Get(context, isolate->NewString(s_localeobjects[i]))));
            v8::Local<v8::Object> object_proto = v8::Local<v8::Object>::Cast(JSValue(object->Get(context, prototype)));
            object_proto->Set(context, toLocaleString, undefined).IsJust();
        }

        v8::Local<v8::Object> date = v8::Local<v8::Object>::Cast(JSValue(global->Get(context, isolate->NewString("Date"))));
        v8::Local<v8::Object> date_proto = v8::Local<v8::Object>::Cast(JSValue(date->Get(context, prototype)));
        date_proto->Set(context, isolate->NewString("toLocaleDateString"), undefined).IsJust();
        date_proto->Set(context, isolate->NewString("toLocaleTimeString"), undefined).IsJust();

        v8::Local<v8::Object> string = v8::Local<v8::Object>::Cast(JSValue(global->Get(context, isolate->NewString("String"))));
        v8::Local<v8::Object> string_proto = v8::Local<v8::Object>::Cast(JSValue(string->Get(context, prototype)));
        string_proto->Set(context, isolate->NewString("localeCompare"), undefined).IsJust();
        string_proto->Set(context, isolate->NewString("toLocaleLowerCase"), undefined).IsJust();
        string_proto->Set(context, isolate->NewString("toLocaleUpperCase"), undefined).IsJust();

        ilog("Initializing JavaScript global objects...");

        global->Set(context, isolate->NewString("action"), action_base::class_info().getModule(isolate)).IsJust();
        global->Set(context, isolate->NewString("console"), bc_console_base::class_info().getModule(isolate)).IsJust();
        global->Set(context, isolate->NewString("crypto"), bc_crypto_base::class_info().getModule(isolate)).IsJust();
        global->Set(context, isolate->NewString("trans"), trans_base::class_info().getModule(isolate)).IsJust();
        global->Set(context, isolate->NewString("db"), Table::tables()).IsJust();

        // from fibjs
        global->Set(context, isolate->NewString("assert"), assert_base::class_info().getModule(isolate)).IsJust();
        Buffer::load_module();

        ilog("Freezing JavaScript global objects...");
        isolate->m_topSandbox->freeze();
    }
}

static result_t emit_event(std::string event)
{
    Isolate* isolate = Isolate::current();

    JSFiber::EnterJsScope s;
    v8::HandleScope handle_scope(isolate->m_isolate);

    JSTrigger t(isolate->m_isolate, chain_base::class_info().getModule(isolate));
    bool r = false;

    result_t hr = t._emit(event, NULL, 0, r);
    if (hr < 0) {
        elog("error occurs when emit ${event} event", ("event", event));
        app().quit();
    }

    return 0;
}

static void chain_main()
{
    Isolate* main_isolate = Isolate::main();
    Isolate* isolate = js_module_interface::g_isolate;

    Runtime rt(isolate);
    v8::Locker locker(isolate->m_isolate);
    v8::Isolate::Scope isolate_scope(isolate->m_isolate);

    v8::HandleScope handle_scope(isolate->m_isolate);
    v8::Context::Scope context_scope(isolate->m_context.Get(isolate->m_isolate));

    JSFiber::EnterJsScope s;

    try {
        SandBox::Scope _scope(isolate->m_topSandbox);
        v8::Unlocker unlocker(isolate->m_isolate);
        std::vector<const char*> s_argv;

        s_argv.resize(s_args.size());
        for (int32_t i = 0; i < (int32_t)s_args.size(); i++)
            s_argv[i] = s_args[i].c_str();

        app().initialize<chain_plugin, net_plugin, producer_plugin, resource_monitor_plugin>(s_argv.size(), (char**)s_argv.data());

        ilog("Trigger the 'load' event...");
        main_isolate->sync([]() -> int { return emit_event("load"); });

        auto& plugin = app().get_plugin<chain_plugin>();
        max_serialization_time = plugin.get_abi_serializer_max_time();
        g_isMainnet = plugin.get_chain_id() == fc::sha256(s_chainid);

        app().startup();

        app().set_thread_priority_max();
        app().exec();

        ilog("Cleaning up the javascript environment...");
    } catch (const fc::exception& e) {
        elog("${e}", ("e", e.to_detail_string()));
        main_isolate->sync([]() -> int { return emit_event("error"); });
    } catch (const chain_exception& e) {
        elog("${e}", ("e", e.to_detail_string()));
        main_isolate->sync([]() -> int { return emit_event("error"); });
    } catch (const boost::interprocess::bad_alloc& e) {
        elog("bad alloc");
        main_isolate->sync([]() -> int { return emit_event("error"); });
    } catch (const boost::exception& e) {
        elog("${e}", ("e", boost::diagnostic_information(e)));
        main_isolate->sync([]() -> int { return emit_event("error"); });
    } catch (const std::runtime_error& e) {
        if (std::string(e.what()) == "database dirty flag set") {
            elog("database dirty flag set (likely due to unclean shutdown): replay required");
        } else if (std::string(e.what()) == "database metadata dirty flag set") {
            elog("database metadata dirty flag set (likely due to unclean shutdown): replay required");
        } else {
            elog("${e}", ("e", e.what()));
        }
        main_isolate->sync([]() -> int { return emit_event("error"); });
    } catch (const std::exception& e) {
        elog("${e}", ("e", e.what()));
        main_isolate->sync([]() -> int { return emit_event("error"); });
    } catch (...) {
        elog("unknown exception");
        main_isolate->sync([]() -> int { return emit_event("error"); });
    }

    ilog("Trigger the 'close' event...");

    main_isolate->sync([]() -> int { return emit_event("close"); });
    main_isolate->Unref();

    ilog("The chain thread is finished running...");
}

void start_chain()
{
    exlib::OSThread th;
    th.bindCurrent();

    s_sem.Wait();

    init_isolate();
    chain_main();

    th.suspend();
}

result_t chain_base::load(exlib::string name, v8::Local<v8::Object> cfg)
{
    exlib::string plug_name = "eosio::" + name + "_plugin";
    auto plugin = app().find_plugin(plug_name.c_str());

    if (!plugin)
        return CHECK_ERROR(Runtime::setError("Plugin " + name + " not found."));

    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();

    if (plugin->get_state() == abstract_plugin::registered) {
        options_description cli, opt;

        plugin->set_program_options(cli, opt);

        s_args.push_back("--plugin");
        s_args.push_back(plug_name);

        JSArray ks = cfg->GetPropertyNames(context);

        int32_t len = ks->Length();
        int32_t i;

        for (i = 0; i < len; i++) {
            JSValue k = ks->Get(context, i);
            JSValue v = cfg->Get(context, k);
            exlib::string key(isolate->toString(k));

            auto opt_item = cli.find_nothrow(key.c_str(), true);
            if (opt_item == NULL)
                opt_item = opt.find_nothrow(key.c_str(), true);
            if (opt_item == NULL)
                return CHECK_ERROR(Runtime::setError("unrecognised option: " + key));

            auto semantic = opt_item->semantic();

            if (semantic->min_tokens() == 0) {
                if (v->IsTrue())
                    s_args.push_back("--" + key);
            } else if (v->IsArray()) {
                const JSArray vals = JSArray::Cast(v);

                if (vals->Length() == 0)
                    return CHECK_ERROR(Runtime::setError("empty option parameters: " + key));

                for (int32_t j = 0; j < vals->Length(); j++) {
                    s_args.push_back("--" + key);
                    JSValue sj = vals->Get(context, j);
                    s_args.push_back(isolate->toString(sj));
                }
            } else {
                s_args.push_back("--" + key);
                s_args.push_back(isolate->toString(v));
            }
        }
    }

    return 0;
}

result_t chain_base::load(v8::Local<v8::Object> cfgs)
{
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();
    JSArray ks = cfgs->GetPropertyNames(context);
    int32_t len = ks->Length();
    int32_t i;
    result_t hr;

    for (i = 0; i < len; i++) {
        JSValue k = ks->Get(context, i);

        v8::Local<v8::Object> o;
        JSValue v = cfgs->Get(context, k);
        hr = GetArgumentValue(isolate, v, o, true);
        if (hr < 0)
            return hr;

        load(isolate->toString(k), o);
    }

    return 0;
}

result_t chain_base::start()
{
    if (s_state_stopped)
        return CHECK_ERROR(Runtime::setError("non-reentrant function"));
    s_state_stopped = true;

    Isolate::main()->Ref();

    UErrorCode status = U_ZERO_ERROR;
    uloc_setDefault("en-US", &status);

    Isolate* isolate = js_module_interface::g_isolate = new Isolate("");
    isolate->m_enable_FileSystem = false;
    isolate->m_console_colored = false;
    isolate->m_safe_buffer = true;
    isolate->m_file_cache.resize(0);

    s_sem.Post();

    return 0;
}

result_t chain_base::stop()
{
    if (s_state_started)
        return CHECK_ERROR(Runtime::setError("non-reentrant function"));
    s_state_started = true;

    app().quit();
    return 0;
}

result_t chain_base::is_dirty()
{
    exlib::string data_dir = app().data_dir().c_str();
    data_dir.append("/state/shared_memory.bin");
    if (!fs::exists(data_dir.c_str())) {
        return 0;
    }
    char header[chainbase::header_size];
    std::ifstream hs(data_dir.c_str(), std::ifstream::binary);
    hs.read(header, chainbase::header_size);
    if (hs.fail()) {
        elog("bad header");
        hs.close();
        return 1;
    }
    chainbase::db_header* dbheader = reinterpret_cast<chainbase::db_header*>(header);
    if (dbheader) {
        if (dbheader->id != chainbase::header_id) {
            data_dir.append(" database format not compatible with this version of chainbase.");
            elog(data_dir.c_str());
            hs.close();
            return 1;
        }
        if (dbheader->dirty) {
            data_dir.append(" database dirty flag set");
            elog(data_dir.c_str());
            hs.close();
            return 1;
        }
        if (dbheader->dbenviron != chainbase::environment()) {
            data_dir.append(" database was created with a chainbase from a different environment");
            elog(data_dir.c_str());
            hs.close();
            return 1;
        }
    }
    hs.close();
    return 0;
}

result_t chain_base::get_name(exlib::string& retVal)
{
    retVal = chain::config::system_account_name.to_string();
    return 0;
}

result_t chain_base::set_name(exlib::string newVal)
{
    exlib::string n;

    n = newVal;
    chain::config::system_account_name = name(std::string_view { n.c_str(), n.length() });

    n = newVal + ".null";
    chain::config::null_account_name = name(std::string_view { n.c_str(), n.length() });

    n = newVal + ".prods";
    chain::config::producers_account_name = name(std::string_view { n.c_str(), n.length() });

    n = newVal + ".token";
    chain::config::token_account_name = name(std::string_view { n.c_str(), n.length() });

    n = newVal + ".auth";
    chain::config::eosio_auth_scope = name(std::string_view { n.c_str(), n.length() });

    n = newVal + ".all";
    chain::config::eosio_all_scope = name(std::string_view { n.c_str(), n.length() });

    n = newVal + ".any";
    chain::config::eosio_any_name = name(std::string_view { n.c_str(), n.length() });

    n = newVal + ".code";
    chain::config::eosio_code_name = name(std::string_view { n.c_str(), n.length() });

    n = newVal + ".cross";
    chain::config::eosio_cross_name = name(std::string_view { n.c_str(), n.length() });

    return 0;
}

result_t chain_base::get_data_dir(exlib::string& retVal)
{
    retVal = app().data_dir().c_str();
    return 0;
}

result_t chain_base::set_data_dir(exlib::string newVal)
{
    app().set_default_data_dir(newVal.c_str());
    return 0;
}

result_t chain_base::get_config_dir(exlib::string& retVal)
{
    retVal = app().config_dir().c_str();
    return 0;
}

result_t chain_base::set_config_dir(exlib::string newVal)
{
    app().set_default_config_dir(newVal.c_str());
    return 0;
}

result_t chain_base::get_log_level(exlib::string& retVal)
{
    switch (fc::logger::get().get_log_level()) {
    case fc::log_level::all:
        retVal = "all";
        break;
    case fc::log_level::debug:
        retVal = "debug";
        break;
    case fc::log_level::info:
        retVal = "info";
        break;
    case fc::log_level::warn:
        retVal = "warn";
        break;
    case fc::log_level::error:
        retVal = "error";
        break;
    case fc::log_level::off:
        retVal = "off";
        break;
    }

    return 0;
}

result_t chain_base::set_log_level(exlib::string newVal)
{
    auto logger = fc::logger::get();

    if (newVal == "all")
        logger.set_log_level(fc::log_level::all);
    else if (newVal == "debug")
        logger.set_log_level(fc::log_level::debug);
    else if (newVal == "info")
        logger.set_log_level(fc::log_level::info);
    else if (newVal == "warn")
        logger.set_log_level(fc::log_level::warn);
    else if (newVal == "error")
        logger.set_log_level(fc::log_level::error);
    else if (newVal == "off")
        logger.set_log_level(fc::log_level::off);
    else
        return CHECK_ERROR(CALL_E_INVALIDARG);

    return 0;
}

result_t chain_base::get_pubkey_prefix(exlib::string& retVal)
{
    retVal = fc::crypto::config::public_key_legacy_prefix;
    return 0;
}

result_t chain_base::set_pubkey_prefix(exlib::string newVal)
{
    static exlib::string s_prefix;

    s_prefix = newVal;
    fc::crypto::config::public_key_legacy_prefix = s_prefix.c_str();
    chain_root_key = (newVal + base_root_key).c_str();

    return 0;
}

result_t chain_base::get_enableJSContract(bool& retVal)
{
    retVal = js_module_interface::g_enable;
    return 0;
}

result_t chain_base::set_enableJSContract(bool newVal)
{
    js_module_interface::g_enable = newVal;
    return 0;
}

} // namespace fibjs
