#ifndef hex_h
#define hex_h

#include "Header.hpp"
namespace fibos {
uint8_t from_hex(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;
    return 0;
}

size_t from_hex(std::string& hex_str, char* out_data, size_t out_data_len)
{
    if (hex_str.length() % 2)
        hex_str.insert(0, "0");
    auto i = hex_str.begin();
    uint8_t* out_pos = (uint8_t*)out_data;
    uint8_t* out_end = out_pos + out_data_len;
    while (i != hex_str.end() && out_end != out_pos) {
        *out_pos = from_hex((char)(*i)) << 4;
        ++i;
        if (i != hex_str.end()) {
            *out_pos |= from_hex((char)(*i));
            ++i;
        }
        ++out_pos;
    }
    return out_pos - (uint8_t*)out_data;
}

hash_256 hex_to_sha256(std::string hex_str)
{
    auto diff = 64 - hex_str.length();
    if (diff > 0) {
        hex_str.insert(0, diff, '0');
    }
    hash_256 checksum;
    from_hex(hex_str, (char*)checksum.hash, sizeof(checksum.hash));
    return checksum;
}

std::string to_hex(const char* d, uint32_t s)
{
    std::string r;
    const char* to_hex = "0123456789abcdef";
    uint8_t* c = (uint8_t*)d;
    for (uint32_t i = 0; i < s; ++i)
        (r += to_hex[(c[i] >> 4)]) += to_hex[(c[i] & 0x0f)];
    return r;
}

std::string sha256_to_hex(const hash_256& sha256)
{
    return to_hex((char*)sha256.hash, sizeof(sha256.hash));
}

std::string string_to_hex(const std::string& input)
{
    static const char* const lut = "0123456789abcdef";
    size_t len = input.length();

    std::string output;
    output.reserve(2 * len);
    for (size_t i = 0; i < len; ++i) {
        const unsigned char c = input[i];
        output.push_back(lut[c >> 4]);
        output.push_back(lut[c & 15]);
    }
    return output;
}
}

#endif /* hex_h */
