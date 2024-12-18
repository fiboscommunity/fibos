#include <fc/io/json.hpp>

#undef stringify_large_ints_and_doubles
#define stringify_large_ints_and_doubles legacy_generator

#include <libfc/src/io/json.cpp>
