#pragma once

#include "I_round_key.hpp"
#include "DES_round_key.hpp"

#include "crypt_type.hpp"

#include <string_view>

class DEAL_128_round_key final : public I_round_key<DES_round_key>
{
    public:
        explicit DEAL_128_round_key(uint128_t);
        explicit DEAL_128_round_key(std::string const &);

        std::vector<DES_round_key> get_round_keys() const override;

    private:
        void generate_round_keys(uint128_t const);

        std::vector<DES_round_key> _round_keys;
};