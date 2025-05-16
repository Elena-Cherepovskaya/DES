#pragma once

#include "I_round_key.hpp"
#include "DES_round_key.hpp"

#include "DES_coder.hpp"
#include "crypt_type.hpp"

class DEAL_128_round_key final : public I_round_key<DES_round_key>
{
    public:
        explicit DEAL_128_round_key(uint128_t);

        std::vector<DES_round_key> const & get_round_keys() const override { return _round_keys; }

    private:
        static byte_array code(DES_coder const & coder, byte_array const &);

        void generate_round_keys(uint128_t const &);

        std::vector<DES_round_key> _round_keys;
};