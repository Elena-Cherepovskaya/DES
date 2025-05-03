#pragma once

#include "I_coder.hpp"

#include "DES_round_key.hpp"
#include "I_coder_function.hpp"

class DES_coder final : public coder_detail<uint64_t>
{
    public:
        DES_coder(uint64_t key, crypp_mode, padding_mode, uint64_t start_vector = 0, uint64_t delta_counter = 1);

        byte_array code(byte_array const & in) override;
        byte_array decode(byte_array const & in) override;
    private:

        void reset_chaining();

        uint64_t begin_crypt_code(uint64_t data);
        uint64_t end_crypt_code(uint64_t data);

        uint64_t begin_crypt_decode(uint64_t data);
        uint64_t end_crypt_decode(uint64_t data);

        DES_round_key _round_key;

        crypp_mode    _crypt_mode;

        uint64_t const _start_vector;
        uint64_t const _delta_counter;
        uint64_t _tmp_vector;
        uint64_t _vector;
};
