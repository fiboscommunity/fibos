#include <libfc/secp256k1/secp256k1/include/secp256k1.h>
#include <fc/crypto/public_key.hpp>

namespace fc {
namespace ecc {
    namespace detail {
        const secp256k1_context* _get_context();
    }
}

namespace crypto {
    typedef fc::array<char, 65> public_key_point_data;
    struct serialize_ecc : public fc::visitor<public_key_point_data> {
        public_key_point_data operator()(const ecc::public_key_shim& key) const
        {
            const secp256k1_context* context = ecc::detail::_get_context();
            auto k = key.serialize();

            secp256k1_pubkey pub_key;
            secp256k1_ec_pubkey_parse(context, &pub_key, (unsigned char*)k.data, 33);

            public_key_point_data fk;
            size_t kOutLen = 65;
            secp256k1_ec_pubkey_serialize(context, (unsigned char*)fk.data, &kOutLen, &pub_key, SECP256K1_EC_UNCOMPRESSED);
            return fk;
        }

        public_key_point_data operator()(const r1::public_key_shim& key) const
        {
            return public_key_point_data();
        }

        public_key_point_data operator()(const webauthn::public_key& key) const
        {
            return public_key_point_data();
        }
    };

    public_key_point_data serialize_ecc_point(public_key key)
    {
        return std::visit(serialize_ecc(), key._storage);
    }
};
};
