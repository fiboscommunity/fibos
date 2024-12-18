#include <cmath>
#include "src/base/platform/platform-posix-time.h"
#include "../include/js_module.h"

namespace v8 {
namespace base {

    const char* PosixDefaultTimezoneCache::LocalTimezone(double time)
    {
        if (fibjs::Isolate::current() == eosio::chain::js_module_interface::g_isolate) {
            return "UTC";
        } else {
            if (std::isnan(time))
                return "";
            time_t tv = static_cast<time_t>(std::floor(time / msPerSecond));
            struct tm tm;
            struct tm* t = localtime_r(&tv, &tm);
            if (!t || !t->tm_zone)
                return "";
            return t->tm_zone;
        }
    }

    double PosixDefaultTimezoneCache::LocalTimeOffset(double time_ms, bool is_utc)
    {
        if (fibjs::Isolate::current() == eosio::chain::js_module_interface::g_isolate) {
            time_t tv = time(nullptr);
            struct tm tm;
            struct tm* t = localtime_r(&tv, &tm);
            return -static_cast<double>(t->tm_isdst > 0 ? 3600 * msPerSecond : 0);
        } else {
            time_t tv = time(nullptr);
            struct tm tm;
            struct tm* t = localtime_r(&tv, &tm);
            // tm_gmtoff includes any daylight savings offset, so subtract it.
            return static_cast<double>(t->tm_gmtoff * msPerSecond - (t->tm_isdst > 0 ? 3600 * msPerSecond : 0));
        }
    }

} // namespace base
} // namespace v8
