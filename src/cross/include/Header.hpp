#pragma once
namespace fibos {
struct hash_256 {
    uint8_t hash[32];

    bool operator==(hash_256 const& rhs) const
    {
        for (int i = 0; i < 32; i++)
            if (this->hash[i] != rhs.hash[i])
                return false;
        return true;
    }
};

struct hash_512 {
    uint8_t hash[64];
};
}
