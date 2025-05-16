#pragma once

#include "I_round_key.hpp"

#include <cstdint>

class DES_round_key final : public I_round_key<uint64_t>
{        
    public:
        explicit DES_round_key(uint64_t key);

        std::vector<uint64_t> const & get_round_keys() const override { return _round_keys; }

    private:
        void generate_round_keys(uint64_t const _key);

        static int constexpr number_round = 16;
        std::vector<uint64_t> _round_keys;
};
