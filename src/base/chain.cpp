#include "config.hpp"

#include "fibjs.h"
#include "object.h"
#include "include/libplatform/libplatform.h"
#include "../js_contract/include/js_module.h"
#include "v8/src/libplatform/default-platform.h"
#include <eosio/chain/transaction_context.hpp>
#include "v8/src/flags/flags.h"

namespace fibjs {
void importModule()
{
    importBuiltinModule();
    IMPORT_MODULE(chain)
}

int init_chain();
void start_chain();
} // namespace fibjs

int32_t main(int32_t argc, char* argv[])
{
    class Platform : public v8::platform::DefaultPlatform {
    public:
        static v8::Platform* platform_creator()
        {
            return new Platform();
        }

        double MonotonicallyIncreasingTime() override
        {
            if (fibjs::Isolate::current() == eosio::chain::js_module_interface::g_isolate && eosio::chain::g_now_context)
                return eosio::chain::g_now_context->control.pending_block_time().sec_since_epoch();
            else
                return v8::platform::DefaultPlatform::MonotonicallyIncreasingTime();
        }

        double CurrentClockTimeMillis() override
        {
            if (fibjs::Isolate::current() == eosio::chain::js_module_interface::g_isolate && eosio::chain::g_now_context)
                return static_cast<uint64_t>(eosio::chain::g_now_context->control.pending_block_time().time_since_epoch().count()) / 1000.0;
            else
                return v8::platform::DefaultPlatform::CurrentClockTimeMillis();
        }
    };

    fibjs::init_chain();
    fibjs::importModule();

    v8::internal::v8_flags.icu_timezone_data = false;

    fibjs::start(argc, argv, fibjs::FiberProcJsEntry, Platform::platform_creator);
    fibjs::start_chain();

    return 0;
}
