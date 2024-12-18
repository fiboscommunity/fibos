#pragma once

#include <vector>
#include <string>
#include <map>

#include "Header.hpp"
#include "hex.hpp"

namespace fibos {
constexpr uint32_t single_prefix_string_start_pos = 0x80;
constexpr uint32_t list_start_pos = 0xc0;
constexpr uint32_t single_prefix_max_length = 0x37;

enum encode_type {
    encode_hex = 1,
    encode_utf8 = 2,
};

class RLP {
public:
    static bool is_immediate_string(const std::string& str)
    {
        if (str.length() == 1 && str[0] < single_prefix_string_start_pos)
            return true;
        return false;
    }

    static bool is_single_prefix_string(const std::string& str)
    {
        if (is_immediate_string(str))
            return false;
        if (str.length() <= single_prefix_max_length)
            return true;
        return false;
    }

    static bool is_multi_prefix_string(const std::string& str)
    {
        if (str.length() > single_prefix_max_length)
            return true;
        return false;
    }

    static std::string generate_string_header(std::string str);
    static std::string encode(std::string str, encode_type type = encode_utf8);
    static std::string encode(hash_256 checksum, encode_type type = encode_utf8);
    static std::string encode(std::vector<char> c, encode_type type = encode_utf8);
    static std::string generate_array_header(std::string str);

    template <typename ArgType>
    static std::string encode(ArgType arg, encode_type type = encode_utf8);
};

std::string RLP::generate_string_header(std::string str)
{
    std::string rlp_header;
    if (is_single_prefix_string(str))
        rlp_header = single_prefix_string_start_pos + str.length();
    else if (is_multi_prefix_string(str)) {
        size_t str_length = str.length();
        std::string length_hash_reserve;
        while (str_length) {
            length_hash_reserve.append(1, str_length % 0x100);
            str_length /= 0x100;
        }
        rlp_header.append(1, single_prefix_string_start_pos + single_prefix_max_length + length_hash_reserve.length());
        rlp_header.insert(rlp_header.end(), length_hash_reserve.rbegin(), length_hash_reserve.rend());
    }
    return rlp_header;
}

std::string RLP::generate_array_header(std::string str)
{
    std::string rlp_header;
    if (str.length() <= single_prefix_max_length)
        rlp_header = list_start_pos + str.length();
    else {
        size_t str_length = str.length();
        std::string length_hash_reserve;
        while (str_length) {
            length_hash_reserve.append(1, str_length % 0x100);
            str_length /= 0x100;
        }
        rlp_header.append(1, list_start_pos + single_prefix_max_length + length_hash_reserve.length());
        rlp_header.insert(rlp_header.end(), length_hash_reserve.rbegin(), length_hash_reserve.rend());
    }
    return rlp_header;
}

std::string RLP::encode(std::string str, encode_type type)
{
    std::string ret_string;
    if (type == encode_hex) {
        std::vector<char> buffer(str.length());
        size_t length = from_hex(str, &buffer[0], buffer.size());
        ret_string.append(std::string(buffer.begin(), buffer.begin() + length));
    } else
        ret_string.append(str);
    ret_string.insert(0, generate_string_header(ret_string));
    return ret_string;
}

std::string RLP::encode(hash_256 checksum, encode_type type)
{
    return encode(sha256_to_hex(checksum), encode_hex);
}

std::string RLP::encode(std::vector<char> c, encode_type type)
{
    return encode(std::string(c.begin(), c.end()), type);
}

template <typename ArgType>
std::string RLP::encode(ArgType arg, encode_type type)
{
    std::string ret_string;
    if (std::is_same<ArgType, std::string>::value) {
        ret_string.append(encode(arg), type);
    } else if (std::is_same<ArgType, hash_256>::value) {
        ret_string.append(encode(arg), type);
    } else if (std::is_same<ArgType, std::vector<char>>::value) {
        ret_string.append(encode(arg), type);
    } else {
        for (int i = 0; i < arg.size(); i++)
            ret_string.append(encode(arg[i], type));
        ret_string.insert(0, generate_array_header(ret_string));
    }
    return ret_string;
}
}
