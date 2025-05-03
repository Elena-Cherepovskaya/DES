#pragma once

#include "I_coder.hpp"

#include "DEAL_128_round_key.hpp"

class DEAL_128_coder final : public coder_detail<uint128_t>
{
    public:
        explicit DEAL_128_coder(uint128_t key, padding_mode);
        explicit DEAL_128_coder(std::string const & key, padding_mode);

        byte_array code(byte_array const & in) override;
        byte_array decode(byte_array const & in) override;

    private:
        DEAL_128_round_key _rk;
};
