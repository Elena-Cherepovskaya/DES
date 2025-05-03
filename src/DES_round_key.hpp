#pragma once

#include "I_round_key.hpp"

#include <cstdint>

class DES_round_key final : public I_round_key<uint64_t>
{        
    public:
        explicit DES_round_key(uint64_t key);

        std::vector<uint64_t> get_round_keys() const override;

    private:
        void generate_round_keys(uint64_t const _key);

        static int const number_round = 16;
        std::vector<uint64_t> _round_keys;
};