#include "object.h"
#include "ifs/bc_console.h"
#include "ifs/console.h"
#include "ifs/assert.h"

namespace fibjs {

result_t bc_console_base::log(exlib::string fmt, OptArgs args)
{
    return console_base::log(fmt, args);
}

result_t bc_console_base::log(OptArgs args)
{
    return console_base::log(args);
}

result_t bc_console_base::debug(exlib::string fmt, OptArgs args)
{
    return console_base::debug(fmt, args);
}

result_t bc_console_base::debug(OptArgs args)
{
    return console_base::debug(args);
}

result_t bc_console_base::info(exlib::string fmt, OptArgs args)
{
    return console_base::info(fmt, args);
}

result_t bc_console_base::info(OptArgs args)
{
    return console_base::info(args);
}

result_t bc_console_base::notice(exlib::string fmt, OptArgs args)
{
    return console_base::notice(fmt, args);
}

result_t bc_console_base::notice(OptArgs args)
{
    return console_base::notice(args);
}

result_t bc_console_base::warn(exlib::string fmt, OptArgs args)
{
    return console_base::warn(fmt, args);
}

result_t bc_console_base::warn(OptArgs args)
{
    return console_base::warn(args);
}

result_t bc_console_base::error(exlib::string fmt, OptArgs args)
{
    return console_base::error(fmt, args);
}

result_t bc_console_base::error(OptArgs args)
{
    return console_base::error(args);
}

result_t bc_console_base::crit(exlib::string fmt, OptArgs args)
{
    return console_base::crit(fmt, args);
}

result_t bc_console_base::crit(OptArgs args)
{
    return console_base::crit(args);
}

result_t bc_console_base::alert(exlib::string fmt, OptArgs args)
{
    return console_base::alert(fmt, args);
}

result_t bc_console_base::alert(OptArgs args)
{
    return console_base::alert(args);
}

result_t bc_console_base::trace(exlib::string fmt, OptArgs args)
{
    return console_base::trace(fmt, args);
}

result_t bc_console_base::trace(OptArgs args)
{
    return console_base::trace(args);
}

result_t bc_console_base::dir(v8::Local<v8::Value> obj, v8::Local<v8::Object> options)
{
    return console_base::dir(obj, options);
}

result_t bc_console_base::_assert(v8::Local<v8::Value> value, exlib::string msg)
{
    return assert_base::ok(value, msg);
}
} // namespace fibjs